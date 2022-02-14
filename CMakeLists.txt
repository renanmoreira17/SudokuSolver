cmake_minimum_required(VERSION 3.12)
project(SudokuSolver VERSION 0.1.0)

set (CMAKE_CXX_STANDARD 20)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -stdlib=libc++ -Wall -Wextra -Wpedantic")

add_subdirectory(src)
add_subdirectory(libs)

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
