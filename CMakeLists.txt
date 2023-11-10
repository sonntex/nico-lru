cmake_minimum_required(VERSION 3.27)

project(nico-lru)

add_compile_options(-Wall)

enable_testing()
include_directories(include)
add_subdirectory(src)
