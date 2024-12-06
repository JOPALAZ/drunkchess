cmake_minimum_required(VERSION 3.16)

project(ChessServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -finline-functions -funroll-loops -fprefetch-loop-arrays -fomit-frame-pointer -march=native -fpermissive")

# Set output directory for the binary
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Collect source files
file(GLOB SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)

# Add executable target
add_executable(chess-server ${SOURCES})

# Ensure the output file does not have .exe extension on Windows
if(WIN32)
    set_target_properties(chess-server PROPERTIES OUTPUT_NAME "chess-server")
    add_custom_command(TARGET chess-server POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E rename ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/chess-server.exe ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/chess-server
        COMMENT "Renaming chess-server.exe to chess-server"
    )
endif()

# Strip debug symbols from the binary
add_custom_command(TARGET chess-server POST_BUILD
    COMMAND ${CMAKE_STRIP} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/chess-server
    COMMENT "Stripping debug symbols from chess-server"
)
