cmake_minimum_required(VERSION 3.12)

file(GLOB Util_SRC CONFIGURE_DEPENDS "*.hpp" "*.cpp")

add_library(Util SHARED ${Util_SRC})

