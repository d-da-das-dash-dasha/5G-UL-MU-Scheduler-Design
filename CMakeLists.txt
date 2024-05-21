cmake_minimum_required(VERSION 3.22.3)
project(MSU)

set(CMAKE_CXX_STANDARD 17)

add_executable(MSU main.cpp
        RBsCounter.cpp
        solver.h solver.cpp)
