#ifndef BITONIC_SORT_H
#define BITONIC_SORT_H

#include <vector>
#include <string>
#include <algorithm> // Required for std::swap

// Forward declaration for different sorting orders
enum class SortOrder {
    Ascending,
    Descending
};

class BitonicSort {
public:
    virtual ~BitonicSort() = default;

    // Pure virtual function to be implemented by derived classes
    virtual void sort(std::vector<int>& arr, SortOrder order) = 0;

    // Helper function to get the name of the sorter (optional, but useful for benchmarks/tests)
    virtual std::string getName() const = 0;

protected:
    // Protected helper for bitonic merge part
    void bitonicMerge(std::vector<int>& arr, int low, int count, SortOrder order) {
        if (count > 1) {
            int k = count / 2;
            for (int i = low; i < low + k; ++i) {
                compareAndSwap(arr, i, i + k, order);
            }
            bitonicMerge(arr, low, k, order);
            bitonicMerge(arr, low + k, k, order);
        }
    }

    // Protected helper for the recursive sort part
    void bitonicSortRecursive(std::vector<int>& arr, int low, int count, SortOrder order) {
        if (count > 1) {
            int k = count / 2;
            // Sort first half in ascending order
            bitonicSortRecursive(arr, low, k, SortOrder::Ascending);
            // Sort second half in descending order
            bitonicSortRecursive(arr, low + k, k, SortOrder::Descending);
            // Merge the whole sequence
            bitonicMerge(arr, low, count, order);
        }
    }

    // Protected helper to compare and swap elements based on order
    void compareAndSwap(std::vector<int>& arr, int i, int j, SortOrder order) {
        bool condition = (order == SortOrder::Ascending) ? (arr[i] > arr[j]) : (arr[i] < arr[j]);
        if (condition) {
            std::swap(arr[i], arr[j]); // Changed to std::swap
        }
    }
};

#endif // BITONIC_SORT_H
