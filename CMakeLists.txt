cmake_minimum_required(VERSION 3.26)
project(lab)

set(CMAKE_CXX_STANDARD 20)

add_executable(lab main.cpp
        Parsing.h
        field.h
        bmp.h)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++ -static-libgcc")
