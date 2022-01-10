cmake_minimum_required(VERSION 3.14)

project(mysticNES LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(mysticNES main.cpp emulator.h emulator.cpp cpu.h cpu.cpp memory.h memory.cpp ppu.h ppu.cpp interrupts.h interrupts.cpp input.h input.cpp)

find_package(SDL2 REQUIRED)
include_directories(mysticNES ${SDL2_INCLUDE_DIRS})

target_link_libraries(mysticNES ${SDL2_LIBRARIES})
