#include "simd_bitonic_sorter.h"
#include <cmath>    // For std::pow, std::log2, std::ceil
#include <limits>   // For std::numeric_limits
#include <iostream> // For debugging
#include <cstdio>   // For printf, if using print_m128i

// Helper to print __m128i for debugging (optional)
// void print_m128i(__m128i var) {
//     alignas(16) int val[4];
//     _mm_store_si128((__m128i*)val, var);
//     printf("SIMD: %d %d %d %d\n", val[0], val[1], val[2], val[3]);
// }

SIMDBitonicSorter::SIMDBitonicSorter() {
    // Constructor can check for CPU support if needed, though CMake should handle arch flags.
}

void SIMDBitonicSorter::padData(std::vector<int>& arr, int& original_size, int& padded_size, SortOrder order) {
    original_size = arr.size();
    if (original_size == 0) {
        padded_size = 0;
        return;
    }
    // Pad to next power of 2 for bitonic sort requirement
    padded_size = std::pow(2, std::ceil(std::log2(original_size)));

    // Further pad to be a multiple of SIMD_WIDTH if necessary, though the merge logic should handle tails.
    // For simplicity, the main padding is to power-of-2. The SIMD merge will handle internal tails.
    if (padded_size > original_size) {
        arr.resize(padded_size, (order == SortOrder::Ascending) ? std::numeric_limits<int>::max() : std::numeric_limits<int>::min());
    }
}

void SIMDBitonicSorter::unpadData(std::vector<int>& arr, int original_size, int padded_size) {
    if (padded_size > original_size) {
        arr.resize(original_size);
    }
}

void SIMDBitonicSorter::sort(std::vector<int>& arr, SortOrder order) {
    if (arr.empty()) {
        return;
    }

    int original_size = 0;
    int padded_size = 0;
    bool is_power_of_two = (arr.size() > 0) && ((arr.size() & (arr.size() - 1)) == 0);

    if (!is_power_of_two) { // Pad if not power of two
        padData(arr, original_size, padded_size, order);
    } else {
        original_size = arr.size();
        padded_size = arr.size();
    }

    // Ensure padded size is appropriate for SIMD operations if it's very small
    // This check might be refined. The main concern is if padded_size < SIMD_WIDTH.
    if (padded_size > 0 && padded_size < SIMD_WIDTH && padded_size < original_size) {
         // If original was small, padding might make it SIMD_WIDTH, then unpadding is fine.
         // If original was tiny (e.g. 1) and padded_size became, say, 4, it's handled.
         // This logic is mainly for the recursive calls.
    }


    if (padded_size > 0) {
       bitonicSortRecursiveSIMD(arr, 0, padded_size, order);
    }

    if (!is_power_of_two && padded_size > 0) {
        unpadData(arr, original_size, padded_size);
    }
}

std::string SIMDBitonicSorter::getName() const {
    return "SIMDBitonicSorter (SSE)"; // Or detect AVX/AVX2 later
}

// This is the core SIMD part for compare & swap of two blocks of SIMD_WIDTH elements
void SIMDBitonicSorter::compareAndSwapBlockSIMD(std::vector<int>& arr, int L_idx, int R_idx, SortOrder order) {
    // Ensure indices are valid and there's enough data for a full SIMD block from each index
    // This function assumes L_idx and R_idx are starting points of SIMD_WIDTH blocks
    // And that arr has enough elements: arr[L_idx+SIMD_WIDTH-1] and arr[R_idx+SIMD_WIDTH-1] are valid

    // Load data from memory (unaligned loads for now for simplicity, can impact performance)
    __m128i block_L = _mm_loadu_si128((__m128i*)&arr[L_idx]);
    __m128i block_R = _mm_loadu_si128((__m128i*)&arr[R_idx]);

    __m128i min_vals, max_vals;

    if (order == SortOrder::Ascending) {
        min_vals = _mm_min_epi32(block_L, block_R); // SSE4.1
        max_vals = _mm_max_epi32(block_L, block_R); // SSE4.1
        _mm_storeu_si128((__m128i*)&arr[L_idx], min_vals);
        _mm_storeu_si128((__m128i*)&arr[R_idx], max_vals);
    } else { // Descending
        min_vals = _mm_min_epi32(block_L, block_R); // SSE4.1
        max_vals = _mm_max_epi32(block_L, block_R); // SSE4.1
        _mm_storeu_si128((__m128i*)&arr[L_idx], max_vals);
        _mm_storeu_si128((__m128i*)&arr[R_idx], min_vals);
    }
    // Note: This simple min/max swap works if block_L elements are compared against block_R elements
    // in a bitonic context where L items should be smaller (or larger) than R items.
    // This is NOT a full sort of 2*SIMD_WIDTH items. It's a direct parallel of:
    // for i=0 to SIMD_WIDTH-1: compareAndSwap(arr, L_idx+i, R_idx+i, order)
}


void SIMDBitonicSorter::bitonicSortRecursiveSIMD(std::vector<int>& arr, int low, int count, SortOrder order) {
    if (count <= 1) {
        return;
    }

    if (count < SEQUENTIAL_THRESHOLD_SIMD) { // Use scalar version for small subproblems
        BitonicSort::bitonicSortRecursive(arr, low, count, order);
        return;
    }

    int k = count / 2;
    // Sort first half in ascending order
    bitonicSortRecursiveSIMD(arr, low, k, SortOrder::Ascending);
    // Sort second half in descending order
    bitonicSortRecursiveSIMD(arr, low + k, k, SortOrder::Descending);

    // Merge the whole sequence
    bitonicMergeSIMD(arr, low, count, order);
}

void SIMDBitonicSorter::bitonicMergeSIMD(std::vector<int>& arr, int low, int count, SortOrder order) {
    if (count <= 1) {
        return;
    }

    // Adjusted threshold for SIMD merge: ensure enough elements for at least one SIMD block comparison.
    if (count < 2 * SIMD_WIDTH || count < SEQUENTIAL_THRESHOLD_SIMD ) {
        BitonicSort::bitonicMerge(arr, low, count, order);
        return;
    }

    int k = count / 2; // k is the distance for comparison

    // Process full SIMD blocks
    // The loop iterates for (i from low to low + k - 1)
    // We process elements in chunks of SIMD_WIDTH.
    // The number of actual compareAndSwap operations is k.
    // So, we iterate k/SIMD_WIDTH times for SIMD blocks.
    int num_simd_ops = k / SIMD_WIDTH;
    for (int i = 0; i < num_simd_ops; ++i) {
        compareAndSwapBlockSIMD(arr, low + i * SIMD_WIDTH, low + i * SIMD_WIDTH + k, order);
    }

    // Handle remaining elements (if k is not a multiple of SIMD_WIDTH) with scalar operations
    int processed_elements = num_simd_ops * SIMD_WIDTH;
    for (int i = processed_elements; i < k; ++i) {
        BitonicSort::compareAndSwap(arr, low + i, low + i + k, order);
    }

    // Recursively call merge on two halves
    bitonicMergeSIMD(arr, low, k, order);
    bitonicMergeSIMD(arr, low + k, k, order);
}
