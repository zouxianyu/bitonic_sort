#include "std_thread_bitonic_sorter.h"
#include <cmath>    // For std::pow, std::log2, std::ceil
#include <limits>   // For std::numeric_limits
#include <iostream> // For debugging
#include <functional> // for std::ref needed for threads

StdThreadBitonicSorter::StdThreadBitonicSorter(unsigned int max_threads)
    : max_threads_(max_threads > 0 ? max_threads : std::thread::hardware_concurrency()) {
    if (max_threads_ == 0) max_threads_ = 1; // Ensure at least one thread
}

void StdThreadBitonicSorter::padData(std::vector<int>& arr, int& original_size, int& padded_size, SortOrder order) {
    original_size = arr.size();
    if (original_size == 0) {
        padded_size = 0;
        return;
    }
    padded_size = std::pow(2, std::ceil(std::log2(original_size)));
    if (padded_size > original_size) {
        // Use min for ascending if padding with smaller numbers helps, max for descending
        // Standard bitonic sort requires power-of-2. Padding ensures this.
        // The values used for padding should not affect the sorted order of original elements.
        // Using INT_MAX for ascending and INT_MIN for descending should generally work
        // if they are not part of the actual dataset extremes.
        arr.resize(padded_size, (order == SortOrder::Ascending) ? std::numeric_limits<int>::max() : std::numeric_limits<int>::min());
    }
}

void StdThreadBitonicSorter::unpadData(std::vector<int>& arr, int original_size, int padded_size) {
    if (padded_size > original_size) {
        arr.resize(original_size);
    }
}

void StdThreadBitonicSorter::sort(std::vector<int>& arr, SortOrder order) {
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

    current_threads_ = 1; // Main thread counts as one
    if (padded_size > 0) {
       bitonicSortRecursiveParallel(arr, 0, padded_size, order, 0);
    }

    if (!is_power_of_two && padded_size > 0) {
        unpadData(arr, original_size, padded_size);
         // After unpadding, if the original order was descending, the padding values (INT_MIN)
         // might be at the end. Re-sort the original range if necessary, or ensure padding
         // logic is robust. For now, assume unpadData is sufficient.
         // A more robust padding would use values guaranteed to be outside the actual data range
         // or handle them carefully.
    }
}

std::string StdThreadBitonicSorter::getName() const {
    return "StdThreadBitonicSorter(max_threads=" + std::to_string(max_threads_) + ")";
}

void StdThreadBitonicSorter::bitonicSortRecursiveParallel(std::vector<int>& arr, int low, int count, SortOrder order, unsigned int depth) {
    if (count <= 1) {
        return;
    }

    int k = count / 2;

    // Determine if we can spawn more threads
    // current_threads_ is a rough estimate of active threads for depth control.
    // A more sophisticated approach might use a thread pool or std::async with futures.
    bool can_spawn_thread = (current_threads_ * 2 <= max_threads_) && (count > SEQUENTIAL_THRESHOLD);

    if (can_spawn_thread) {
        current_threads_ += 2; // Tentatively increment for the two potential new threads

        std::thread t1(&StdThreadBitonicSorter::bitonicSortRecursiveParallel, this, std::ref(arr), low, k, SortOrder::Ascending, depth + 1);
        // Sort second half in descending order (this is standard bitonic step)
        bitonicSortRecursiveParallel(arr, low + k, k, SortOrder::Descending, depth + 1);
        t1.join();

        current_threads_ -= 2; // Decrement after threads are done
    } else {
        // Sequential execution for this level
        bitonicSortRecursive(arr, low, k, SortOrder::Ascending); // Uses base class sequential version
        bitonicSortRecursive(arr, low + k, k, SortOrder::Descending); // Uses base class sequential version
    }

    // Merge the whole sequence (parallel or sequential)
    bitonicMergeParallel(arr, low, count, order, depth);
}

void StdThreadBitonicSorter::bitonicMergeParallel(std::vector<int>& arr, int low, int count, SortOrder order, unsigned int depth) {
    if (count <= 1) {
        return;
    }

    int k = count / 2;
    for (int i = low; i < low + k; ++i) {
        compareAndSwap(arr, i, i + k, order);
    }

    bool can_spawn_thread = (current_threads_ * 2 <= max_threads_) && (count > SEQUENTIAL_THRESHOLD);

    if (can_spawn_thread) {
        current_threads_ += 2;

        std::thread t1(&StdThreadBitonicSorter::bitonicMergeParallel, this, std::ref(arr), low, k, order, depth + 1);
        bitonicMergeParallel(arr, low + k, k, order, depth + 1);
        t1.join();

        current_threads_ -= 2;
    } else {
        bitonicMerge(arr, low, k, order); // Uses base class sequential version
        bitonicMerge(arr, low + k, k, order); // Uses base class sequential version
    }
}
