cmake_minimum_required(VERSION 3.10)
project(BitonicSort LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# vcpkg integration
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake"
  CACHE STRING "Vcpkg toolchain file")

# Enable testing
enable_testing()

# Add subdirectories
add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(benchmarks)

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    # Linking will be done in src/CMakeLists.txt and tests/CMakeLists.txt
    message(STATUS "OpenMP found. Linking will be handled in subdirectories.")
else()
    message(WARNING "OpenMP not found. OpenMPBitonicSorter may not work correctly.")
endif() # End OpenMP block

# SIMD Compiler Flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(PROJECT_SIMD_FLAGS "-msse4.1")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(PROJECT_SIMD_FLAGS "/arch:AVX")
endif()

if(PROJECT_SIMD_FLAGS)
    message(STATUS "Project-level SIMD compile flags configured: ${PROJECT_SIMD_FLAGS}")
else()
    message(WARNING "Project-level SIMD flags not set for this compiler.")
endif()
