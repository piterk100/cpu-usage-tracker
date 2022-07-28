cmake_minimum_required(VERSION 3.22.1)

add_compile_options(-g)
add_compile_options(-Wall -Wextra)

project(CUT)

add_executable(CUT main.c
        functions.c
        functions.h)

set_property(TARGET CUT PROPERTY C_STANDARD 99)