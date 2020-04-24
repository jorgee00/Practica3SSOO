cmake_minimum_required(VERSION 3.15)
project(ssoo_p3_initial_code)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")

include_directories(.)

add_executable(ssoo_p3_initial_code
        corrector_ssoo_p3.sh
        costCalculator.c
        Makefile
        queue.c
        queue.h)
