# Project CMakeLists.txt

cmake_minimum_required(VERSION 3.10)

project(Audio360 VERSION 1.0 LANGUAGES C CXX ASM)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(LOGGING_ENABLED ON)

option(ARM_BUILD "ON to build for ARM target." OFF)
option(BUILD_TESTS "ON to build tests job." OFF)

add_subdirectory(src)

if(BUILD_TESTS)
    add_subdirectory(test)
endif()
