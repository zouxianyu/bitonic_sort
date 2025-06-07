cmake_minimum_required(VERSION 3.10)
project(BitonicSortBenchmarks)

find_package(benchmark CONFIG REQUIRED) # Use CONFIG mode for google-benchmark via vcpkg
find_package(OpenMP) # To ensure OpenMP flags are available if needed by sorters

# Add benchmark executable
add_executable(run_benchmarks benchmark_main.cpp)

target_link_libraries(run_benchmarks PRIVATE
    benchmark::benchmark # Link against google-benchmark
    bitonic_sorters      # Link against our sorters library
)

if(OpenMP_CXX_FOUND)
    target_link_libraries(run_benchmarks PRIVATE OpenMP::OpenMP_CXX)
endif()

# Optional: Add to CTest
# include(GoogleTest)
# add_test(
#   NAME ${PROJECT_NAME}_benchmarks
#   COMMAND run_benchmarks --benchmark_filter=all --benchmark_min_time=0.01 # Basic run
# )
# Disabling add_test for now, will run manually to capture CSV.
