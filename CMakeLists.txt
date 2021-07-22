cmake_minimum_required(VERSION 3.19)
project(Memory_Allocator C)

set(CMAKE_C_STANDARD 99)

add_executable(Memory_Allocator main.c source/allocator.c source/cache.c source/slab.c source/slot.c)

target_include_directories(Memory_Allocator PUBLIC include)

find_library(Random random lib)
target_link_libraries(Memory_Allocator LINK_PUBLIC ${Random})
