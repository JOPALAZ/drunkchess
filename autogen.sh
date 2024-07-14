#!/bin/bash
g++ ./src/*.cpp -O3 -finline-functions -funroll-loops -fprefetch-loop-arrays -fomit-frame-pointer -march=x86-64 -fpermissive -o ./bin/chess-server
strip ./bin/chess-server 