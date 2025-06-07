#include "openmp_bitonic_sorter.h"
#include <cmath>    // For std::pow, std::log2, std::ceil
#include <limits>   // For std::numeric_limits
#include <iostream> // For debugging

OpenMPBitonicSorter::OpenMPBitonicSorter() {
    // Optionally, set number of threads if not relying on environment variable or default
    // For example: omp_set_num_threads(omp_get_max_threads());
    // But usually, it's better to let OpenMP manage this unless specific control is needed.
}

void OpenMPBitonicSorter::padData(std::vector<int>& arr, int& original_size, int& padded_size, SortOrder order) {
    original_size = arr.size();
    if (original_size == 0) {
        padded_size = 0;
        return;
    }
    padded_size = std::pow(2, std::ceil(std::log2(original_size)));
    if (padded_size > original_size) {
        arr.resize(padded_size, (order == SortOrder::Ascending) ? std::numeric_limits<int>::max() : std::numeric_limits<int>::min());
    }
}

void OpenMPBitonicSorter::unpadData(std::vector<int>& arr, int original_size, int padded_size) {
    if (padded_size > original_size) {
        arr.resize(original_size);
    }
}

void OpenMPBitonicSorter::sort(std::vector<int>& arr, SortOrder order) {
    if (arr.empty()) {
        return;
    }

    int original_size = 0;
    int padded_size = 0;
    bool is_power_of_two = (arr.size() > 0) && ((arr.size() & (arr.size() - 1)) == 0);

    if (!is_power_of_two) {
        padData(arr, original_size, padded_size, order);
    } else {
        original_size = arr.size();
        padded_size = arr.size();
    }

    if (padded_size > 0) {
        // Enclose the initial call in a parallel region to enable nested tasking if desired
        // However, tasks created with 'task' directive are part of the current task region.
        // A single parallel region at the top level is often sufficient.
        // Pass correct 'low' which is 0 for the initial call.
        #pragma omp parallel default(none) shared(arr, padded_size, order) if(padded_size > SEQUENTIAL_THRESHOLD_OMP)
        {
            #pragma omp single nowait
            {
                bitonicSortRecursiveOMP(arr, 0, padded_size, order);
            }
        } // Implicit barrier here
    }

    if (!is_power_of_two && padded_size > 0) {
        unpadData(arr, original_size, padded_size);
    }
}

std::string OpenMPBitonicSorter::getName() const {
    // Could try to get omp_get_max_threads() here, but it might vary
    return "OpenMPBitonicSorter";
}

void OpenMPBitonicSorter::bitonicSortRecursiveOMP(std::vector<int>& arr, int low, int count, SortOrder order) {
    if (count <= 1) {
        return;
    }

    int k = count / 2;

    if (count > SEQUENTIAL_THRESHOLD_OMP) {
        // Using OpenMP tasks for recursive calls
        #pragma omp task default(none) shared(arr, low, k, order)
        {
            bitonicSortRecursiveOMP(arr, low, k, SortOrder::Ascending);
        }
        // Pass correct 'low + k' for the second half.
        #pragma omp task default(none) shared(arr, low, k, order)
        {
            bitonicSortRecursiveOMP(arr, low + k, k, SortOrder::Descending);
        }
        #pragma omp taskwait // Wait for the two sorting tasks to complete before merging

        bitonicMergeOMP(arr, low, count, order);

    } else {
        // Use base class sequential versions for small subproblems
        BitonicSort::bitonicSortRecursive(arr, low, k, SortOrder::Ascending);
        BitonicSort::bitonicSortRecursive(arr, low + k, k, SortOrder::Descending);
        BitonicSort::bitonicMerge(arr, low, count, order);
    }
}

void OpenMPBitonicSorter::bitonicMergeOMP(std::vector<int>& arr, int low, int count, SortOrder order) {
    if (count <= 1) {
        return;
    }

    int k = count / 2;
    // The loop for compareAndSwap can be parallelized if k is large enough
    // However, bitonic merge itself is recursive. Parallelizing the loop here might be
    // too fine-grained if the recursive calls are already tasked.
    // For now, keeping this loop sequential within each task.
    for (int i = low; i < low + k; ++i) {
        compareAndSwap(arr, i, i + k, order);
    }

    if (count > SEQUENTIAL_THRESHOLD_OMP) {
        #pragma omp task default(none) shared(arr, low, k, order)
        {
            bitonicMergeOMP(arr, low, k, order);
        }
        // Pass correct 'low + k' for the second half.
        #pragma omp task default(none) shared(arr, low, k, order)
        {
            bitonicMergeOMP(arr, low + k, k, order);
        }
        // #pragma omp taskwait // Not strictly needed here if the merge is the last thing in the calling task
                                // and the calling task has a taskwait. But for clarity or safety:
        #pragma omp taskwait
    } else {
        BitonicSort::bitonicMerge(arr, low, k, order);
        BitonicSort::bitonicMerge(arr, low + k, k, order);
    }
}
