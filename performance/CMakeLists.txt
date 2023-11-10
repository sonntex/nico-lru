cmake_minimum_required(VERSION 3.27.7)

add_executable(performance
  performance.cpp)

target_link_libraries(performance)

add_test(NAME performance COMMAND performance)
