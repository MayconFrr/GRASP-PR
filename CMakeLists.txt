cmake_minimum_required(VERSION 3.16)
project(GRASP-PR C)

set(CMAKE_C_STANDARD 11)

add_executable(main src/main.c src/knapsack.c)