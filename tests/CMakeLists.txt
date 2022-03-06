cmake_minimum_required(VERSION 3.5)

set(catch2_SOURCE_DIR ${CMAKE_SOURCE_DIR}/libs/Catch2)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

include(CTest)
include(Catch)

add_subdirectory(Board)
add_subdirectory(Solver)
