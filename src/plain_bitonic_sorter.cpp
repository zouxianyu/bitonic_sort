#include "plain_bitonic_sorter.h"
#include <cmath> // For std::pow, std::log2, std::ceil
#include <limits> // For std::numeric_limits

void PlainBitonicSorter::padData(std::vector<int>& arr, int& original_size, int& padded_size, SortOrder order) { // Add SortOrder parameter
    original_size = arr.size();
    if (original_size == 0) {
        padded_size = 0;
        return;
    }
    // Calculate the next power of 2
    padded_size = std::pow(2, std::ceil(std::log2(original_size)));
    if (padded_size > original_size) {
        // Pad with a value larger than any expected element for ascending sort,
        // or smaller for descending sort.
        int padding_value = (order == SortOrder::Ascending) ? std::numeric_limits<int>::max() : std::numeric_limits<int>::min();
        arr.resize(padded_size, padding_value);
    }
}

void PlainBitonicSorter::unpadData(std::vector<int>& arr, int original_size, int padded_size) {
    if (padded_size > original_size) {
        arr.resize(original_size);
    }
}

void PlainBitonicSorter::sort(std::vector<int>& arr, SortOrder order) {
    if (arr.empty()) {
        return;
    }

    int original_size = 0;
    int padded_size = 0;

    // Check if size is a power of 2. If not, pad it.
    bool is_power_of_two = (arr.size() > 0) && ((arr.size() & (arr.size() - 1)) == 0);

    if (!is_power_of_two) {
        // original_size and padded_size are passed by reference and set in padData
        padData(arr, original_size, padded_size, order);
    } else {
        original_size = arr.size();
        padded_size = arr.size();
    }

    if (padded_size > 0) { // Ensure array is not empty after padding
      bitonicSortRecursive(arr, 0, padded_size, order);
    }

    if (!is_power_of_two && padded_size > 0) { // Ensure unpadding only if padding happened and array was not empty
        unpadData(arr, original_size, padded_size);
    }
}

std::string PlainBitonicSorter::getName() const {
    return "PlainBitonicSorter";
}
