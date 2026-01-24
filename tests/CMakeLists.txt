cmake_minimum_required(VERSION 3.5)

include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.8.0
    )
    
FetchContent_MakeAvailable(Catch2)

list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)
    
include(CTest)
include(Catch)

add_subdirectory(Board)
add_subdirectory(Solver)
