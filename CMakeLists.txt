# Project CMakeLists.txt

cmake_minimum_required(VERSION 3.10)

project(Audio360 VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(BUILD_TESTS ON)

add_subdirectory(src)

if(BUILD_TESTS)
    add_subdirectory(test)
endif()
