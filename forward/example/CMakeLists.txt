cmake_minimum_required(VERSION 2.8)

project(example)

include_directories(../include)
aux_source_directory(../src FORWARDSRC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -march=native -O3 -pthread")
SET(EXECUTABLE_OUTPUT_PATH ../output/bin)

add_executable(example
        main.cc
        ${FORWARDSRC}
        )
