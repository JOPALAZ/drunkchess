#!/bin/bash
g++ ./src/*.cpp -O3 -finline-functions -funroll-loops -fprefetch-loop-arrays -fomit-frame-pointer -march=native -fpermissive -o ./bin/chess-server
strip ./bin/chess-server 