cmake_minimum_required(VERSION 3.10)

find_package(GTest REQUIRED)

# Add test executable
# This will be populated with test files later
add_executable(run_tests main.cpp test_plain_sorter.cpp test_std_thread_sorter.cpp test_openmp_sorter.cpp test_simd_sorter.cpp)
target_link_libraries(run_tests PRIVATE GTest::GTest GTest::Main bitonic_sorters)
if(OpenMP_CXX_FOUND)
    target_link_libraries(run_tests PUBLIC OpenMP::OpenMP_CXX)
endif()
include(GoogleTest)
gtest_discover_tests(run_tests)
