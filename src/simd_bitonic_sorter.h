#ifndef SIMD_BITONIC_SORTER_H
#define SIMD_BITONIC_SORTER_H

#include "bitonic_sort.h"
#include <vector>
#include <string>
#include <algorithm> // For std::min, std::is_sorted
#include <stdexcept> // For std::invalid_argument

// Include header for x86 intrinsics
#include <immintrin.h>

class SIMDBitonicSorter : public BitonicSort {
public:
    SIMDBitonicSorter();
    ~SIMDBitonicSorter() override = default;

    void sort(std::vector<int>& arr, SortOrder order) override;
    std::string getName() const override;

private:
    // SSE processes 4 integers at a time (128 bits / 32 bits per int)
    static const int SIMD_WIDTH = 4;
    // AVX2 would be 8, AVX512 would be 16

    // Threshold for switching to sequential sort for small subproblems
    // or for parts not large enough for effective SIMD.
    static const int SEQUENTIAL_THRESHOLD_SIMD = 64; // Must be at least 2*SIMD_WIDTH for some operations

    void bitonicSortRecursiveSIMD(std::vector<int>& arr, int low, int count, SortOrder order);
    void bitonicMergeSIMD(std::vector<int>& arr, int low, int count, SortOrder order);

    // SIMD specific compare and swap for a block of elements
    void compareAndSwapBlockSIMD(std::vector<int>& arr, int L_idx, int R_idx, SortOrder order);

    void padData(std::vector<int>& arr, int& original_size, int& padded_size, SortOrder order);
    void unpadData(std::vector<int>& arr, int original_size, int padded_size);
};

#endif // SIMD_BITONIC_SORTER_H
