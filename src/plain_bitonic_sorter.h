#ifndef PLAIN_BITONIC_SORTER_H
#define PLAIN_BITONIC_SORTER_H

#include "bitonic_sort.h"
#include <vector>
#include <string>
#include <algorithm> // For std::is_sorted and std::sort (for correctness check in main, not in sort itself)
#include <stdexcept> // For std::invalid_argument

class PlainBitonicSorter : public BitonicSort {
public:
    PlainBitonicSorter() = default;
    ~PlainBitonicSorter() override = default;

    void sort(std::vector<int>& arr, SortOrder order) override;
    std::string getName() const override;

private:
    // The recursive sort and merge functions are already in the base class
    // and can be called directly if arr.size() is a power of 2.
    // This implementation will handle non-power-of-2 sizes by padding.
    void padData(std::vector<int>& arr, int& original_size, int& padded_size, SortOrder order); // Add SortOrder parameter
    void unpadData(std::vector<int>& arr, int original_size, int padded_size);
};

#endif // PLAIN_BITONIC_SORTER_H
