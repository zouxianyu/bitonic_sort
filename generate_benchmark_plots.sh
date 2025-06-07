#!/bin/bash
set -e

echo "Creating directories..."
mkdir -p utils
mkdir -p doc/figures

echo "Making the Python script executable (optional, as we call it with python)..."
chmod +x utils/plot_benchmarks.py

echo "Running the Python script to generate plots..."
# Ensure python3 is used if available, otherwise python might be python2
if command -v python3 &>/dev/null; then
    PYTHON_EXEC=python3
else
    PYTHON_EXEC=python
fi

$PYTHON_EXEC utils/plot_benchmarks.py

echo "Listing generated figures:"
ls -l doc/figures/

echo "Python script execution completed."
