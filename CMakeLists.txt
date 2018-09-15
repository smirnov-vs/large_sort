cmake_minimum_required(VERSION 2.8)
project(sort)

add_compile_options(-O3)
add_compile_options(-std=c++17)

include_directories(common)

add_library(common STATIC common/helpers.cpp)
link_libraries(common)

add_executable(sort sort/main.cpp)

add_executable(generator generator/main.cpp)
