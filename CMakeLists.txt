cmake_minimum_required(VERSION 3.25.1)
project(SIK_1 C)

set(CMAKE_C_STANDARD 17)


add_executable(ppcbs ppcbs.c err.c)
add_executable(ppcbc ppcbc.c err.c)

