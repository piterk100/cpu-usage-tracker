cmake_minimum_required(VERSION 3.22.1)
add_compile_options(-g)
project(CUT)

add_executable(CUT main.c
        functions.c
        functions.h)