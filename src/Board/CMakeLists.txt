cmake_minimum_required(VERSION 3.12)

file(GLOB Board_SRC CONFIGURE_DEPENDS "*.hpp" "*.cpp")

add_library(Board SHARED ${Board_SRC})

target_link_libraries(Board PUBLIC fmt Util)
