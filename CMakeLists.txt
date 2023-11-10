cmake_minimum_required(VERSION 3.27.7)

project(nico-lru)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -O3")

enable_testing()

include_directories(include)

add_subdirectory(performance)
