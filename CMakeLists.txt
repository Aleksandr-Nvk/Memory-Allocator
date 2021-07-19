cmake_minimum_required(VERSION 3.19)
project(Memory_Allocator C)

set(CMAKE_C_STANDARD 99)

add_executable(Memory_Allocator main.c allocator.c allocator.h)
