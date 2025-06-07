#ifndef OPENMP_BITONIC_SORTER_H
#define OPENMP_BITONIC_SORTER_H

#include "bitonic_sort.h"
#include <vector>
#include <string>
#include <algorithm> // For std::min
#include <stdexcept> // For std::invalid_argument
// No specific OpenMP header needed for most directives, but omp.h can be used for runtime functions like omp_get_max_threads()
#include <omp.h>

class OpenMPBitonicSorter : public BitonicSort {
public:
    OpenMPBitonicSorter(); // Constructor can set default num_threads if needed, or rely on OMP_NUM_THREADS
    ~OpenMPBitonicSorter() override = default;

    void sort(std::vector<int>& arr, SortOrder order) override;
    std::string getName() const override;

private:
    // Threshold for switching to sequential sort
    static const int SEQUENTIAL_THRESHOLD_OMP = 1024; // Potentially tune this

    void bitonicSortRecursiveOMP(std::vector<int>& arr, int low, int count, SortOrder order);
    void bitonicMergeOMP(std::vector<int>& arr, int low, int count, SortOrder order);

    void padData(std::vector<int>& arr, int& original_size, int& padded_size, SortOrder order);
    void unpadData(std::vector<int>& arr, int original_size, int padded_size);
};

#endif // OPENMP_BITONIC_SORTER_H
