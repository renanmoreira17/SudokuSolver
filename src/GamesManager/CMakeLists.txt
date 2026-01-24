cmake_minimum_required(VERSION 3.12)

file(GLOB Games_SRC CONFIGURE_DEPENDS "*.hpp" "*.cpp")

add_library(Games ${Games_SRC})
target_include_directories(Games PUBLIC ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(Games PUBLIC project_options PRIVATE project_warnings)

