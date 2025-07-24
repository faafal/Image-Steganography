cmake_minimum_required(VERSION 3.30)
project(ImageSteganography)

set(CMAKE_CXX_STANDARD 20)

add_executable(ImageSteganography main.cpp
        bmpProcessor.cpp
        ppmProcessor.cpp
        helpFunctions.cpp)
