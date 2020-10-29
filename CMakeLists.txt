cmake_minimum_required(VERSION 3.16)
project(PFS_173_ROMPLER C)

set(CMAKE_C_STANDARD 99)

add_executable(dummy_PFS_173_ROMPLER src/rompler.c)
add_executable(dummy_PFS173_SINETEST src/sinetest.c)
include_directories(include)