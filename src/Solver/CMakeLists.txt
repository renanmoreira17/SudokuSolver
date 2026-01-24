cmake_minimum_required(VERSION 3.12)

file(GLOB_RECURSE Solver_SRC CONFIGURE_DEPENDS "*.hpp" "*.cpp")

add_library(Solver ${Solver_SRC})

target_include_directories(Solver PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_link_libraries(Solver PUBLIC Board project_options PRIVATE project_warnings)
