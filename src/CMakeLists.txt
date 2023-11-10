cmake_minimum_required(VERSION 3.27)

add_executable(load-tests load_tests.cpp)
set_property(TARGET load-tests PROPERTY CXX_STANDARD 20)
target_link_libraries(load-tests)
