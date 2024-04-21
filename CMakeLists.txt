cmake_minimum_required(VERSION 3.25.1)
project(SIK_1 C)

set(CMAKE_C_STANDARD 17)


add_executable(ppcbs ppcbs.c err.c common.c err.h common.h)
add_executable(ppcbc ppcbc.c err.c common.c err.h common.h)

target_compile_options(ppcbs PRIVATE -Wall -Wextra -O2 -ggdb3)
target_compile_options(ppcbc PRIVATE -Wall -Wextra -ggdb3)