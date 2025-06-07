#include "gtest/gtest.h"
#include "plain_bitonic_sorter.h" // Adjust path as necessary
#include <vector>
#include <algorithm> // For std::is_sorted, std::sort
#include <iostream>  // For debugging output if needed

// Helper function to print vectors for debugging
static void printVector(const std::vector<int>& vec, const std::string& label = "") {
    if (!label.empty()) {
        std::cout << label << ": ";
    }
    for (int x : vec) {
        std::cout << x << " ";
    }
    std::cout << std::endl;
}

TEST(PlainBitonicSorterTest, EmptyVector) {
    PlainBitonicSorter sorter;
    std::vector<int> vec = {};
    std::vector<int> expected = {};
    sorter.sort(vec, SortOrder::Ascending);
    EXPECT_EQ(vec, expected);
    sorter.sort(vec, SortOrder::Descending);
    EXPECT_EQ(vec, expected);
}

TEST(PlainBitonicSorterTest, SingleElementVector) {
    PlainBitonicSorter sorter;
    std::vector<int> vec = {1};
    std::vector<int> expected = {1};
    sorter.sort(vec, SortOrder::Ascending);
    EXPECT_EQ(vec, expected);
    sorter.sort(vec, SortOrder::Descending);
    EXPECT_EQ(vec, expected);
}

TEST(PlainBitonicSorterTest, PowerOfTwoSizeAscending) {
    PlainBitonicSorter sorter;
    std::vector<int> vec = {3, 7, 4, 8, 6, 2, 1, 5}; // 8 elements
    std::vector<int> expected = {1, 2, 3, 4, 5, 6, 7, 8};
    sorter.sort(vec, SortOrder::Ascending);
    EXPECT_EQ(vec, expected);
}

TEST(PlainBitonicSorterTest, PowerOfTwoSizeDescending) {
    PlainBitonicSorter sorter;
    std::vector<int> vec = {3, 7, 4, 8, 6, 2, 1, 5}; // 8 elements
    std::vector<int> expected = {8, 7, 6, 5, 4, 3, 2, 1};
    sorter.sort(vec, SortOrder::Descending);
    EXPECT_EQ(vec, expected);
}

TEST(PlainBitonicSorterTest, NonPowerOfTwoSizeAscending) {
    PlainBitonicSorter sorter;
    std::vector<int> vec = {3, 7, 4, 8, 6, 2, 1}; // 7 elements
    std::vector<int> expected = {1, 2, 3, 4, 6, 7, 8};
    // printVector(vec, "Original NP2 Asc");
    sorter.sort(vec, SortOrder::Ascending);
    // printVector(vec, "Sorted NP2 Asc");
    // printVector(expected, "Expected NP2 Asc");
    EXPECT_EQ(vec, expected);
}

TEST(PlainBitonicSorterTest, NonPowerOfTwoSizeDescending) {
    PlainBitonicSorter sorter;
    std::vector<int> vec = {3, 7, 4, 8, 6, 2, 1}; // 7 elements
    std::vector<int> expected = {8, 7, 6, 4, 3, 2, 1};
    sorter.sort(vec, SortOrder::Descending);
    EXPECT_EQ(vec, expected);
}

TEST(PlainBitonicSorterTest, AlreadySortedAscending) {
    PlainBitonicSorter sorter;
    std::vector<int> vec = {1, 2, 3, 4, 5};
    std::vector<int> expected = {1, 2, 3, 4, 5};
    sorter.sort(vec, SortOrder::Ascending);
    EXPECT_EQ(vec, expected);
}

TEST(PlainBitonicSorterTest, AlreadySortedDescending) {
    PlainBitonicSorter sorter;
    std::vector<int> vec = {5, 4, 3, 2, 1};
    std::vector<int> expected = {5, 4, 3, 2, 1};
    sorter.sort(vec, SortOrder::Descending);
    EXPECT_EQ(vec, expected);
}

TEST(PlainBitonicSorterTest, ReverseSortedAscending) {
    PlainBitonicSorter sorter;
    std::vector<int> vec = {5, 4, 3, 2, 1};
    std::vector<int> expected = {1, 2, 3, 4, 5};
    sorter.sort(vec, SortOrder::Ascending);
    EXPECT_EQ(vec, expected);
}

TEST(PlainBitonicSorterTest, ReverseSortedDescending) {
    PlainBitonicSorter sorter;
    std::vector<int> vec = {1, 2, 3, 4, 5};
    std::vector<int> expected = {5, 4, 3, 2, 1};
    sorter.sort(vec, SortOrder::Descending);
    EXPECT_EQ(vec, expected);
}

TEST(PlainBitonicSorterTest, DuplicateElementsAscending) {
    PlainBitonicSorter sorter;
    std::vector<int> vec = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5}; // 11 elements
    std::vector<int> expected = {1, 1, 2, 3, 3, 4, 5, 5, 5, 6, 9};
    sorter.sort(vec, SortOrder::Ascending);
    EXPECT_EQ(vec, expected);
}

TEST(PlainBitonicSorterTest, DuplicateElementsDescending) {
    PlainBitonicSorter sorter;
    std::vector<int> vec = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5}; // 11 elements
    std::vector<int> expected = {9, 6, 5, 5, 5, 4, 3, 3, 2, 1, 1};
    sorter.sort(vec, SortOrder::Descending);
    EXPECT_EQ(vec, expected);
}

TEST(PlainBitonicSorterTest, AllSameElements) {
    PlainBitonicSorter sorter;
    std::vector<int> vec = {7, 7, 7, 7, 7};
    std::vector<int> expected = {7, 7, 7, 7, 7};
    sorter.sort(vec, SortOrder::Ascending);
    EXPECT_EQ(vec, expected);
    sorter.sort(vec, SortOrder::Descending);
    EXPECT_EQ(vec, expected);
}

TEST(PlainBitonicSorterTest, LargerDatasetAscending) {
    PlainBitonicSorter sorter;
    std::vector<int> vec = {10, 2, 7, 5, 8, 1, 9, 3, 4, 6, 12, 11, 15, 13, 16, 14}; // 16 elements
    std::vector<int> expected = vec;
    std::sort(expected.begin(), expected.end());
    sorter.sort(vec, SortOrder::Ascending);
    EXPECT_EQ(vec, expected);
}

TEST(PlainBitonicSorterTest, LargerDatasetDescending) {
    PlainBitonicSorter sorter;
    std::vector<int> vec = {10, 2, 7, 5, 8, 1, 9, 3, 4, 6, 12, 11, 15, 13, 16, 14}; // 16 elements
    std::vector<int> expected = vec;
    std::sort(expected.begin(), expected.end(), std::greater<int>());
    sorter.sort(vec, SortOrder::Descending);
    EXPECT_EQ(vec, expected);
}
