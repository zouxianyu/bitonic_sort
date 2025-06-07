#!/bin/bash
set -e

# Create tests/test_simd_sorter.cpp
cat << 'EOF' > tests/test_simd_sorter.cpp
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

EOF

# Update tests/CMakeLists.txt
# Add test_simd_sorter.cpp to the run_tests executable
# This is a bit tricky with sed because of the existing structure.
# Let's try to add it after main.cpp. A more robust way would be to parse and reconstruct.
# For now, assuming a relatively standard format of add_executable.

# Check if test_simd_sorter.cpp is already there
if grep -q "test_simd_sorter.cpp" tests/CMakeLists.txt; then
    echo "test_simd_sorter.cpp already in tests/CMakeLists.txt"
else
    # Add test_simd_sorter.cpp to the add_executable line.
    # This command finds the line with "add_executable(run_tests" that also contains "test_openmp_sorter.cpp",
    # and then replaces "test_openmp_sorter.cpp)" with "test_openmp_sorter.cpp test_simd_sorter.cpp)"
    sed -i '/add_executable(run_tests.*test_openmp_sorter.cpp)/s/test_openmp_sorter.cpp)/test_openmp_sorter.cpp test_simd_sorter.cpp)/' tests/CMakeLists.txt

    if grep -q "test_simd_sorter.cpp" tests/CMakeLists.txt; then
        echo "Successfully added test_simd_sorter.cpp to tests/CMakeLists.txt"
    else
        echo "Failed to add test_simd_sorter.cpp to tests/CMakeLists.txt. Please check manually."
        exit 1
    fi

    # Verify the change
    echo "--- tests/CMakeLists.txt content after modification ---"
    cat tests/CMakeLists.txt
    echo "-------------------------------------------------------"
fi

# Now, run the build and test script again.
# Ensure build directory is clean for configuration.
rm -rf build

# Using the specific CMake version from vcpkg downloads
echo "Configuring project with CMake..."
./vcpkg/downloads/tools/cmake-3.30.1-linux/cmake-3.30.1-linux-x86_64/bin/cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=./vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_BUILD_TYPE=Debug

echo "Building project with CMake..."
# Using the specific CMake version from vcpkg downloads
./vcpkg/downloads/tools/cmake-3.30.1-linux/cmake-3.30.1-linux-x86_64/bin/cmake --build build --clean-first

# Run tests using ctest
cd build
echo "Current directory: $(pwd)"
echo "Listing files in build directory:"
ls -la
echo "Running ctest for SIMD tests..."
# Using the specific ctest version from vcpkg downloads
# We expect more tests now. The previous run had 39. SIMD adds 21 tests. So 39+21 = 60 tests.
/app/vcpkg/downloads/tools/cmake-3.30.1-linux/cmake-3.30.1-linux-x86_64/bin/ctest --output-on-failure
cd ..
echo "SIMD test implementation and execution script completed."

# The second robust sed block is removed as the first one should be sufficient and correct now.

echo "Subtask finished."
