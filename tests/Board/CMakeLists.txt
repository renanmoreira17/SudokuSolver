cmake_minimum_required(VERSION 3.12)

file(GLOB BoardTests_SRC CONFIGURE_DEPENDS "*.cpp")

add_executable(BoardTests ${BoardTests_SRC})
target_link_libraries(BoardTests PRIVATE Catch2::Catch2WithMain Board)
catch_discover_tests(BoardTests)

