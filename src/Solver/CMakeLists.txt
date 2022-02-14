cmake_minimum_required(VERSION 3.12)

file(GLOB_RECURSE Solver_SRC CONFIGURE_DEPENDS "*.hpp" "*.cpp")

add_library(Solver ${Solver_SRC})

target_link_libraries(Solver PRIVATE Board Util)
