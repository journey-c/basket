cmake_minimum_required (VERSION 2.8)

project(Example)

find_package(Threads)
include_directories(../include)
aux_source_directory(../src SRC)

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -march=native -O3 -pthread" )

add_executable(Example
        main.cc
	${SRC}
)
