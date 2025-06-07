#!/bin/bash
set -e

echo "Creating directories..."
mkdir -p benchmarks
mkdir -p doc/data

echo "Building and running benchmarks..."
# Configure
./vcpkg/downloads/tools/cmake-3.30.1-linux/cmake-3.30.1-linux-x86_64/bin/cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=./vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_BUILD_TYPE=Release # Use Release for benchmarks

# Build (clean first to ensure benchmark executable is picked up)
./vcpkg/downloads/tools/cmake-3.30.1-linux/cmake-3.30.1-linux-x86_64/bin/cmake --build build --target run_benchmarks --clean-first

# Run benchmarks and output to CSV
echo "Running benchmarks and saving to doc/data/performance_results.csv"
./build/benchmarks/run_benchmarks --benchmark_format=csv --benchmark_out=doc/data/performance_results.csv --benchmark_out_format=csv

# Display first few lines of CSV for confirmation
echo "CSV Output Head:"
head -n 15 doc/data/performance_results.csv

echo "Benchmark script completed."
