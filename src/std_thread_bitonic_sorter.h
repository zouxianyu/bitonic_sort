#ifndef STD_THREAD_BITONIC_SORTER_H
#define STD_THREAD_BITONIC_SORTER_H

#include "bitonic_sort.h"
#include <vector>
#include <string>
#include <thread>
#include <algorithm> // For std::min
#include <stdexcept> // For std::invalid_argument

class StdThreadBitonicSorter : public BitonicSort {
public:
    // Constructor allows specifying max threads, defaults to hardware concurrency
    StdThreadBitonicSorter(unsigned int max_threads = std::thread::hardware_concurrency());
    ~StdThreadBitonicSorter() override = default;

    void sort(std::vector<int>& arr, SortOrder order) override;
    std::string getName() const override;

private:
    unsigned int max_threads_;
    unsigned int current_threads_ = 0; // To keep track of active threads for simple depth control

    // Threshold for switching to sequential sort for small subproblems
    static const int SEQUENTIAL_THRESHOLD = 1024; // Potentially tune this

    void bitonicSortRecursiveParallel(std::vector<int>& arr, int low, int count, SortOrder order, unsigned int depth);
    void bitonicMergeParallel(std::vector<int>& arr, int low, int count, SortOrder order, unsigned int depth);

    void padData(std::vector<int>& arr, int& original_size, int& padded_size, SortOrder order);
    void unpadData(std::vector<int>& arr, int original_size, int padded_size);
};

#endif // STD_THREAD_BITONIC_SORTER_H
