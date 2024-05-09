cmake_minimum_required(VERSION 3.27)
project(project_3 C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(project_3
        blockmapping.h
        flashdevicedriver.c
        ftl.c
        main.c)
