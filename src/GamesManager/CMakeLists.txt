cmake_minimum_required(VERSION 3.12)

file(GLOB Games_SRC CONFIGURE_DEPENDS "*.hpp" "*.cpp")

add_library(Games SHARED ${Games_SRC})
target_include_directories(Games PUBLIC ${CMAKE_CURRENT_LIST_DIR})

