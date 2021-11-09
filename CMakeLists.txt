cmake_minimum_required(VERSION 3.14)

project(mysticNES LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(mysticNES main.cpp emulator.h emulator.cpp cpu.h cpu.cpp memory.h memory.cpp )
