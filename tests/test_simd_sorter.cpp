#include "gtest/gtest.h"
#include "simd_bitonic_sorter.h"
#include "bitonic_sort.h" // For SortOrder
#include <vector>
#include <algorithm> // For std::is_sorted, std::sort, std::generate, std::reverse
#include <random>    // For std::mt19937, std::uniform_int_distribution

// Helper function to print vector contents (for debugging)
void printVector(const std::vector<int>& arr, const std::string& label = "") {
    if (!label.empty()) {
        std::cout << label << ": ";
    }
    for (int x : arr) {
        std::cout << x << " ";
    }
    std::cout << std::endl;
}

// Test fixture for SIMD Bitonic Sorter
class SIMDBitonicSorterTest : public ::testing::Test {
protected:
    SIMDBitonicSorter sorter;
    std::vector<int> arr;
    std::vector<int> sorted_arr;

    void SetUp(const std::vector<int>& input_arr) {
        arr = input_arr;
        sorted_arr = input_arr;
    }

    void checkSort(SortOrder order) {
        sorter.sort(arr, order);
        if (order == SortOrder::Ascending) {
            std::sort(sorted_arr.begin(), sorted_arr.end());
            ASSERT_EQ(arr.size(), sorted_arr.size());
            EXPECT_TRUE(std::is_sorted(arr.begin(), arr.end()));
            EXPECT_EQ(arr, sorted_arr) << "Ascending sort failed.";
        } else {
            std::sort(sorted_arr.begin(), sorted_arr.end(), std::greater<int>());
            ASSERT_EQ(arr.size(), sorted_arr.size());
            EXPECT_TRUE(std::is_sorted(arr.begin(), arr.end(), std::greater<int>()));
            EXPECT_EQ(arr, sorted_arr) << "Descending sort failed.";
        }
    }

    void generateRandomVector(size_t size, int min_val = -1000, int max_val = 1000) {
        arr.resize(size);
        std::mt19937 gen(42); // Fixed seed for reproducibility
        std::uniform_int_distribution<> distrib(min_val, max_val);
        std::generate(arr.begin(), arr.end(), [&]() { return distrib(gen); });
        sorted_arr = arr;
    }
};

// Test cases
TEST_F(SIMDBitonicSorterTest, EmptyArray) {
    SetUp({});
    checkSort(SortOrder::Ascending);
    SetUp({});
    checkSort(SortOrder::Descending);
}

TEST_F(SIMDBitonicSorterTest, SingleElement) {
    SetUp({5});
    checkSort(SortOrder::Ascending);
    SetUp({5});
    checkSort(SortOrder::Descending);
}

TEST_F(SIMDBitonicSorterTest, AlreadySortedAscending) {
    SetUp({1, 2, 3, 4, 5, 6, 7, 8});
    checkSort(SortOrder::Ascending);
}

TEST_F(SIMDBitonicSorterTest, AlreadySortedDescending) {
    SetUp({8, 7, 6, 5, 4, 3, 2, 1});
    checkSort(SortOrder::Descending);
}

TEST_F(SIMDBitonicSorterTest, ReverseOrderToSortAscending) {
    SetUp({8, 7, 6, 5, 4, 3, 2, 1});
    checkSort(SortOrder::Ascending);
}

TEST_F(SIMDBitonicSorterTest, ReverseOrderToSortDescending) {
    SetUp({1, 2, 3, 4, 5, 6, 7, 8});
    checkSort(SortOrder::Descending);
}

TEST_F(SIMDBitonicSorterTest, RandomSmallArrayPowerOfTwo) {
    // Size = 8 (SIMD_WIDTH * 2)
    generateRandomVector(8);
    checkSort(SortOrder::Ascending);
    generateRandomVector(8);
    checkSort(SortOrder::Descending);
}

TEST_F(SIMDBitonicSorterTest, RandomSmallArrayNotPowerOfTwo) {
    // Size = 7
    generateRandomVector(7);
    checkSort(SortOrder::Ascending);
    generateRandomVector(7);
    checkSort(SortOrder::Descending);
}

TEST_F(SIMDBitonicSorterTest, RandomMediumArrayPowerOfTwo) {
    // Size = 64 (SEQUENTIAL_THRESHOLD_SIMD)
    generateRandomVector(64);
    checkSort(SortOrder::Ascending);
    generateRandomVector(64);
    checkSort(SortOrder::Descending);
}

TEST_F(SIMDBitonicSorterTest, RandomMediumArrayNotPowerOfTwo) {
    // Size = 60
    generateRandomVector(60);
    checkSort(SortOrder::Ascending);
    generateRandomVector(60);
    checkSort(SortOrder::Descending);
}

TEST_F(SIMDBitonicSorterTest, RandomLargeArrayPowerOfTwo) {
    // Size = 256
    generateRandomVector(256);
    checkSort(SortOrder::Ascending);
    generateRandomVector(256);
    checkSort(SortOrder::Descending);
}

TEST_F(SIMDBitonicSorterTest, RandomLargeArrayNotPowerOfTwo) {
    // Size = 250
    generateRandomVector(250);
    checkSort(SortOrder::Ascending);
    generateRandomVector(250);
    checkSort(SortOrder::Descending);
}

TEST_F(SIMDBitonicSorterTest, ArrayWithDuplicates) {
    SetUp({5, 2, 8, 2, 5, 9, 0, 0, 5});
    checkSort(SortOrder::Ascending);
    SetUp({5, 2, 8, 2, 5, 9, 0, 0, 5});
    checkSort(SortOrder::Descending);
}

TEST_F(SIMDBitonicSorterTest, ArrayWithNegativeNumbers) {
    SetUp({-5, 2, -8, 0, 5, -2});
    checkSort(SortOrder::Ascending);
    SetUp({-5, 2, -8, 0, 5, -2});
    checkSort(SortOrder::Descending);
}

TEST_F(SIMDBitonicSorterTest, ArraySizeLessThanSIMDWidth) {
    // Size = 3 (less than SIMD_WIDTH=4)
    generateRandomVector(3);
    checkSort(SortOrder::Ascending);
    generateRandomVector(3);
    checkSort(SortOrder::Descending);
}

TEST_F(SIMDBitonicSorterTest, ArraySizeEqualToSIMDWidth) {
    // Size = 4 (equal to SIMD_WIDTH=4)
    generateRandomVector(4);
    checkSort(SortOrder::Ascending);
    generateRandomVector(4);
    checkSort(SortOrder::Descending);
}

TEST_F(SIMDBitonicSorterTest, ArraySizeSlightlyLargerThanSIMDWidth) {
    // Size = 5
    generateRandomVector(5);
    checkSort(SortOrder::Ascending);
    generateRandomVector(5);
    checkSort(SortOrder::Descending);
}

TEST_F(SIMDBitonicSorterTest, ArraySizeBetweenSIMDWidthAndSequentialThreshold) {
    // Size = 30 (SIMD_WIDTH < 30 < SEQUENTIAL_THRESHOLD_SIMD)
    generateRandomVector(30);
    checkSort(SortOrder::Ascending);
    generateRandomVector(30);
    checkSort(SortOrder::Descending);
}

TEST_F(SIMDBitonicSorterTest, ArraySizeAroundSequentialThreshold) {
    // Size = SEQUENTIAL_THRESHOLD_SIMD -1, +0, +1
    generateRandomVector(SIMDBitonicSorter::SEQUENTIAL_THRESHOLD_SIMD - 1);
    checkSort(SortOrder::Ascending);
    generateRandomVector(SIMDBitonicSorter::SEQUENTIAL_THRESHOLD_SIMD);
    checkSort(SortOrder::Ascending);
    generateRandomVector(SIMDBitonicSorter::SEQUENTIAL_THRESHOLD_SIMD + 1);
    checkSort(SortOrder::Ascending);
}

TEST_F(SIMDBitonicSorterTest, StressTestLargeRandom) {
    // Size = 1024 + some extra to make it not power of two (e.g. 1024 + 7 = 1031)
    // This will be padded to 2048.
    generateRandomVector(1031, -10000, 10000);
    checkSort(SortOrder::Ascending);
    generateRandomVector(1031, -10000, 10000);
    checkSort(SortOrder::Descending);
}

TEST_F(SIMDBitonicSorterTest, MinMaxValues) {
    SetUp({std::numeric_limits<int>::max(), 0, std::numeric_limits<int>::min(), 42, -100});
    checkSort(SortOrder::Ascending);
    SetUp({std::numeric_limits<int>::max(), 0, std::numeric_limits<int>::min(), 42, -100});
    checkSort(SortOrder::Descending);
}
