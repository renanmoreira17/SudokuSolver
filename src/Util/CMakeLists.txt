cmake_minimum_required(VERSION 3.12)

file(GLOB Util_SRC CONFIGURE_DEPENDS "*.hpp" "*.cpp")

add_library(Util ${Util_SRC})

target_link_libraries(Util PUBLIC GlobalDefinitions)
