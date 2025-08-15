cmake_minimum_required(VERSION 3.10)
project(clox C)

set(CMAKE_C_COMPILER gcc)

# Collect all C source files
file(GLOB SOURCES "*.c")

add_executable(clox ${SOURCES})

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
    target_compile_options(clox PRIVATE -g)
endif()

# Add warnings for safety
target_compile_options(clox PRIVATE -Wall -Wextra -pedantic)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(clox PRIVATE -O3 -march=native -flto)
endif()
