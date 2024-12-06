#!/bin/bash
mkdir build
cd build
cmake ..
make -j`nproc`
cd ..
rm -rf build
