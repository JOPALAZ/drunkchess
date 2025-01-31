#!/bin/bash

# Create a build directory
mkdir build
cd build

# Check if the $OS variable equals Windows_NT (an indicator of a Windows environment)
if [[ "$OS" == "Windows_NT" ]]; then
    cmake .. -G "MinGW Makefiles" -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++
else
    cmake ..
fi

# Run the build
make -j"$(nproc)"

# Go back to the root directory and remove the build folder
cd ..
rm -rf build
