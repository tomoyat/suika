cmake_minimum_required(VERSION 3.16)
project(suika)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(lib)

add_executable(suika main.cpp)
target_link_libraries(suika lib)
