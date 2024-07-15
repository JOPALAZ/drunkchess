g++ ./src/*.cpp -O3 -finline-functions -funroll-loops -fprefetch-loop-arrays -fomit-frame-pointer -march=native -fpermissive -o ./bin/chess-server
cd bin
del chess-server
ren chess-server.exe chess-server
cd ..
strip ./bin/chess-server 