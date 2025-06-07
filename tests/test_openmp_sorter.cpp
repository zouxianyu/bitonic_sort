#include "gtest/gtest.h"
#include "openmp_bitonic_sorter.h" // Adjust path as necessary
#include <vector>
#include <algorithm> // For std::is_sorted, std::sort, std::generate, std::greater
#include <iostream>  // For debugging
#include <omp.h>     // For omp_get_max_threads() if needed for info
#include <cstdlib>   // For std::rand

// Helper function to print vectors (can be reused or defined in a common test utility)
static void printVectorTestOMP(const std::vector<int>& vec, const std::string& label = "") {
    if (!label.empty()) {
        // std::cout << label << ": ";
    }
    // for (int x : vec) {
    //     std::cout << x << " ";
    // }
    // std::cout << std::endl;
}

class OpenMPBitonicSorterTest : public ::testing::Test {
protected:
    OpenMPBitonicSorter sorter_;

    void run_sort_test(std::vector<int>& vec, SortOrder order) {
        std::vector<int> expected = vec;
        if (order == SortOrder::Ascending) {
            std::sort(expected.begin(), expected.end());
        } else {
            std::sort(expected.begin(), expected.end(), std::greater<int>());
        }

        // printVectorTestOMP(vec, "Original (" + sorter_.getName() + ", OMP Threads: " + std::to_string(omp_get_max_threads()) +")");
        sorter_.sort(vec, order);
        // printVectorTestOMP(vec, "Sorted (" + sorter_.getName() + ")");
        // printVectorTestOMP(expected, "Expected (" + sorter_.getName() + ")");

        EXPECT_EQ(vec, expected);
    }
};


TEST_F(OpenMPBitonicSorterTest, EmptyVector) {
    std::vector<int> vec = {};
    run_sort_test(vec, SortOrder::Ascending);
    run_sort_test(vec, SortOrder::Descending);
}

TEST_F(OpenMPBitonicSorterTest, SingleElementVector) {
    std::vector<int> vec = {1};
    run_sort_test(vec, SortOrder::Ascending);
    run_sort_test(vec, SortOrder::Descending);
}

TEST_F(OpenMPBitonicSorterTest, PowerOfTwoSizeAscending) {
    std::vector<int> vec = {3, 7, 4, 8, 6, 2, 1, 5}; // 8 elements
    run_sort_test(vec, SortOrder::Ascending);
}

TEST_F(OpenMPBitonicSorterTest, PowerOfTwoSizeDescending) {
    std::vector<int> vec = {3, 7, 4, 8, 6, 2, 1, 5}; // 8 elements
    run_sort_test(vec, SortOrder::Descending);
}

TEST_F(OpenMPBitonicSorterTest, NonPowerOfTwoSizeAscending) {
    std::vector<int> vec = {3, 7, 4, 8, 6, 2, 1}; // 7 elements
    run_sort_test(vec, SortOrder::Ascending);
}

TEST_F(OpenMPBitonicSorterTest, NonPowerOfTwoSizeDescending) {
    std::vector<int> vec = {3, 7, 4, 8, 6, 2, 1}; // 7 elements
    run_sort_test(vec, SortOrder::Descending);
}

TEST_F(OpenMPBitonicSorterTest, AlreadySortedAscending) {
    std::vector<int> vec = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20}; // 20 elements
    run_sort_test(vec, SortOrder::Ascending);
}

TEST_F(OpenMPBitonicSorterTest, ReverseSortedAscending) {
    std::vector<int> vec = {20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1}; // 20 elements
    run_sort_test(vec, SortOrder::Ascending);
}

TEST_F(OpenMPBitonicSorterTest, DuplicateElementsAscending) {
    std::vector<int> vec = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5, 8, 9, 7, 9, 3, 2, 3, 8, 4, 6}; // 21 elements
    run_sort_test(vec, SortOrder::Ascending);
}

TEST_F(OpenMPBitonicSorterTest, DuplicateElementsDescending) {
    std::vector<int> vec = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5, 8, 9, 7, 9, 3, 2, 3, 8, 4, 6}; // 21 elements
    run_sort_test(vec, SortOrder::Descending);
}


TEST_F(OpenMPBitonicSorterTest, LargerRandomDataset) {
    std::vector<int> vec(2048); // Power of 2
    std::generate(vec.begin(), vec.end(), std::rand);
    run_sort_test(vec, SortOrder::Ascending);

    std::generate(vec.begin(), vec.end(), std::rand);
    run_sort_test(vec, SortOrder::Descending);

    std::vector<int> vec_non_power_2(1500);
    std::generate(vec_non_power_2.begin(), vec_non_power_2.end(), std::rand);
    run_sort_test(vec_non_power_2, SortOrder::Ascending);

    std::generate(vec_non_power_2.begin(), vec_non_power_2.end(), std::rand);
    run_sort_test(vec_non_power_2, SortOrder::Descending);
}

TEST_F(OpenMPBitonicSorterTest, StressTestLargeSize) {
    std::vector<int> vec(1 << 14); // 16384 elements
    std::generate(vec.begin(), vec.end(), std::rand);
    run_sort_test(vec, SortOrder::Ascending);
}
