#!/bin/bash
# Exit immediately if a command exits with a non-zero status.
set -e

# 0. Clean up previous vcpkg directory to ensure fresh clone and bootstrap
echo "Removing existing vcpkg directory if present..."
rm -rf vcpkg build # Also remove previous build directory

# 1. Clone vcpkg
echo "Cloning vcpkg..."
git clone https://github.com/microsoft/vcpkg.git || { echo "git clone failed"; exit 1; }
echo "vcpkg cloned."

# 2. Bootstrap vcpkg
echo "Bootstrapping vcpkg..."
./vcpkg/bootstrap-vcpkg.sh -disableMetrics || { cat ./bootstrap-vcpkg.log; echo "bootstrap failed"; exit 1; }
echo "vcpkg bootstrapped."

# 3. Install dependencies using vcpkg (will read vcpkg.json)
echo "Installing dependencies using vcpkg..."
./vcpkg/vcpkg install || { echo "vcpkg install failed"; exit 1; }
echo "Dependencies installed."

# 4. Configure the project using CMake
echo "Configuring project with CMake..."
# Using the specific CMake version downloaded by vcpkg
./vcpkg/downloads/tools/cmake-3.30.1-linux/cmake-3.30.1-linux-x86_64/bin/cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=./vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_BUILD_TYPE=Debug || { echo "CMake configuration failed"; exit 1; }
echo "CMake configuration done."

# 5. Build the project
echo "Building project with CMake..."
# Using the specific CMake version downloaded by vcpkg
./vcpkg/downloads/tools/cmake-3.30.1-linux/cmake-3.30.1-linux-x86_64/bin/cmake --build build || { echo "CMake build failed"; exit 1; }
echo "CMake build done."

# 6. Run tests using ctest
echo "Running tests with CTest..."
cd build || { echo "Failed to cd into build directory"; exit 1; }
echo "Current directory: $(pwd)"
echo "Listing files in build directory:"
ls -la
echo "Running ctest..."
# Using an absolute-like path to ctest from /app
/app/vcpkg/downloads/tools/cmake-3.30.1-linux/cmake-3.30.1-linux-x86_64/bin/ctest --output-on-failure || { echo "CTest execution failed"; exit 1; }
cd ..
echo "Build and test script completed."
