cmake_minimum_required(VERSION 3.10)

# Add library target for sorting algorithms
# This will be populated later
add_library(bitonic_sorters plain_bitonic_sorter.cpp plain_bitonic_sorter.h bitonic_sort.h std_thread_bitonic_sorter.cpp std_thread_bitonic_sorter.h openmp_bitonic_sorter.cpp openmp_bitonic_sorter.h simd_bitonic_sorter.cpp simd_bitonic_sorter.h)
target_include_directories(bitonic_sorters INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(bitonic_sorters PUBLIC "-msse4.1")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(bitonic_sorters PUBLIC "/arch:AVX")
endif()
if(OpenMP_CXX_FOUND)
    target_link_libraries(bitonic_sorters PUBLIC OpenMP::OpenMP_CXX)
endif()
