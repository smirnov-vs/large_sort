cmake_minimum_required(VERSION 2.8)
project(sort)

add_compile_options(-O3)
add_compile_options(-std=c++17)

add_executable(sort sort/main.cpp)

add_executable(generator generator/main.cpp)
