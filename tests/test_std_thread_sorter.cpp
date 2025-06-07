#include "gtest/gtest.h"
#include "std_thread_bitonic_sorter.h" // Adjust path as necessary
#include <vector>
#include <algorithm> // For std::is_sorted, std::sort, std::generate, std::greater
#include <iostream>  // For debugging
#include <cstdlib>   // For std::rand

// Helper function to print vectors (can be reused or defined in a common test utility)
static void printVectorTest(const std::vector<int>& vec, const std::string& label = "") {
    if (!label.empty()) {
        std::cout << label << ": ";
    }
    for (int x : vec) {
        std::cout << x << " ";
    }
    std::cout << std::endl;
}

class StdThreadBitonicSorterTest : public ::testing::Test {
protected:
    // Test with default number of threads (usually hardware_concurrency)
    StdThreadBitonicSorter sorter_default_threads_;
    // Test with a specific number of threads (e.g., 4)
    StdThreadBitonicSorter sorter_4_threads_{4};
    // Test with 1 thread (should behave like sequential)
    StdThreadBitonicSorter sorter_1_thread_{1};


    void run_sort_test(StdThreadBitonicSorter& sorter, std::vector<int>& vec, SortOrder order) {
        std::vector<int> expected = vec;
        if (order == SortOrder::Ascending) {
            std::sort(expected.begin(), expected.end());
        } else {
            std::sort(expected.begin(), expected.end(), std::greater<int>());
        }

        // printVectorTest(vec, "Original (" + sorter.getName() + ")");
        sorter.sort(vec, order);
        // printVectorTest(vec, "Sorted (" + sorter.getName() + ")");
        // printVectorTest(expected, "Expected (" + sorter.getName() + ")");

        EXPECT_EQ(vec, expected);
    }
};


TEST_F(StdThreadBitonicSorterTest, EmptyVector) {
    std::vector<int> vec = {};
    run_sort_test(sorter_default_threads_, vec, SortOrder::Ascending);
    run_sort_test(sorter_default_threads_, vec, SortOrder::Descending);
    run_sort_test(sorter_4_threads_, vec, SortOrder::Ascending);
    run_sort_test(sorter_1_thread_, vec, SortOrder::Ascending);
}

TEST_F(StdThreadBitonicSorterTest, SingleElementVector) {
    std::vector<int> vec = {1};
    run_sort_test(sorter_default_threads_, vec, SortOrder::Ascending);
    run_sort_test(sorter_default_threads_, vec, SortOrder::Descending);
}

TEST_F(StdThreadBitonicSorterTest, PowerOfTwoSizeAscending) {
    std::vector<int> vec = {3, 7, 4, 8, 6, 2, 1, 5}; // 8 elements
    run_sort_test(sorter_default_threads_, vec, SortOrder::Ascending);
    vec = {3, 7, 4, 8, 6, 2, 1, 5};
    run_sort_test(sorter_4_threads_, vec, SortOrder::Ascending);
    vec = {3, 7, 4, 8, 6, 2, 1, 5};
    run_sort_test(sorter_1_thread_, vec, SortOrder::Ascending);
}

TEST_F(StdThreadBitonicSorterTest, PowerOfTwoSizeDescending) {
    std::vector<int> vec = {3, 7, 4, 8, 6, 2, 1, 5}; // 8 elements
    run_sort_test(sorter_default_threads_, vec, SortOrder::Descending);
    vec = {3, 7, 4, 8, 6, 2, 1, 5};
    run_sort_test(sorter_4_threads_, vec, SortOrder::Descending);
     vec = {3, 7, 4, 8, 6, 2, 1, 5};
    run_sort_test(sorter_1_thread_, vec, SortOrder::Descending);
}

TEST_F(StdThreadBitonicSorterTest, NonPowerOfTwoSizeAscending) {
    std::vector<int> vec = {3, 7, 4, 8, 6, 2, 1}; // 7 elements
    run_sort_test(sorter_default_threads_, vec, SortOrder::Ascending);
     vec = {3, 7, 4, 8, 6, 2, 1};
    run_sort_test(sorter_4_threads_, vec, SortOrder::Ascending);
    vec = {3, 7, 4, 8, 6, 2, 1};
    run_sort_test(sorter_1_thread_, vec, SortOrder::Ascending);
}

TEST_F(StdThreadBitonicSorterTest, NonPowerOfTwoSizeDescending) {
    std::vector<int> vec = {3, 7, 4, 8, 6, 2, 1}; // 7 elements
    run_sort_test(sorter_default_threads_, vec, SortOrder::Descending);
    vec = {3, 7, 4, 8, 6, 2, 1};
    run_sort_test(sorter_4_threads_, vec, SortOrder::Descending);
    vec = {3, 7, 4, 8, 6, 2, 1};
    run_sort_test(sorter_1_thread_, vec, SortOrder::Descending);
}

TEST_F(StdThreadBitonicSorterTest, AlreadySortedAscending) {
    std::vector<int> vec = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20}; // 20 elements
    run_sort_test(sorter_default_threads_, vec, SortOrder::Ascending);
}

TEST_F(StdThreadBitonicSorterTest, ReverseSortedAscending) {
    std::vector<int> vec = {20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1}; // 20 elements
    run_sort_test(sorter_default_threads_, vec, SortOrder::Ascending);
}

TEST_F(StdThreadBitonicSorterTest, DuplicateElementsAscending) {
    std::vector<int> vec = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5, 8, 9, 7, 9, 3, 2, 3, 8, 4, 6}; // 21 elements
    run_sort_test(sorter_default_threads_, vec, SortOrder::Ascending);
    vec = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5, 8, 9, 7, 9, 3, 2, 3, 8, 4, 6};
    run_sort_test(sorter_4_threads_, vec, SortOrder::Ascending);
}

TEST_F(StdThreadBitonicSorterTest, DuplicateElementsDescending) {
    std::vector<int> vec = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5, 8, 9, 7, 9, 3, 2, 3, 8, 4, 6}; // 21 elements
    run_sort_test(sorter_default_threads_, vec, SortOrder::Descending);
    vec = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5, 8, 9, 7, 9, 3, 2, 3, 8, 4, 6};
    run_sort_test(sorter_4_threads_, vec, SortOrder::Descending);
}


TEST_F(StdThreadBitonicSorterTest, LargerRandomDataset) {
    std::vector<int> vec(2048); // Power of 2, good for threading
    std::generate(vec.begin(), vec.end(), std::rand);
    run_sort_test(sorter_default_threads_, vec, SortOrder::Ascending);

    std::generate(vec.begin(), vec.end(), std::rand);
    run_sort_test(sorter_default_threads_, vec, SortOrder::Descending);

    std::vector<int> vec_non_power_2(1500);
    std::generate(vec_non_power_2.begin(), vec_non_power_2.end(), std::rand);
    run_sort_test(sorter_4_threads_, vec_non_power_2, SortOrder::Ascending);

    std::generate(vec_non_power_2.begin(), vec_non_power_2.end(), std::rand);
    run_sort_test(sorter_4_threads_, vec_non_power_2, SortOrder::Descending);
}

TEST_F(StdThreadBitonicSorterTest, MaxRecursionDepthTest) {
    // Test with a size that would typically exceed simple recursion depth if not handled by thresholding or careful threading
    std::vector<int> vec(1 << 13); // 8192 elements
    std::generate(vec.begin(), vec.end(), std::rand);
    run_sort_test(sorter_default_threads_, vec, SortOrder::Ascending); // Default threads

    // Test with 1 thread, should be mostly sequential due to thread limit
    std::generate(vec.begin(), vec.end(), std::rand);
    run_sort_test(sorter_1_thread_, vec, SortOrder::Ascending);
}
