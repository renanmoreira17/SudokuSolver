cmake_minimum_required(VERSION 3.12)

file(GLOB SolverTests_SRC CONFIGURE_DEPENDS "*.cpp")

add_executable(SolverTests ${SolverTests_SRC})
target_link_libraries(SolverTests PRIVATE Catch2::Catch2WithMain Solver)
catch_discover_tests(SolverTests)

