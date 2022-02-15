cmake_minimum_required(VERSION 3.12)

file(GLOB Board_SRC CONFIGURE_DEPENDS "*.hpp" "*.cpp")

add_library(Board SHARED ${Board_SRC})

target_include_directories(Board PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_link_libraries(Board PUBLIC fmt Util)
