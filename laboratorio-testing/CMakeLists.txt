cmake_minimum_required(VERSION 3.14)

project(LaboratorioTesting)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Opciones de cobertura
option(ENABLE_COVERAGE "Enable coverage reporting" OFF)

if(ENABLE_COVERAGE)
    message(STATUS "Coverage enabled")
    add_compile_options(--coverage -O0 -g)
    add_link_options(--coverage)
endif()

include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/release-1.12.1.zip
)

# Evita problemas en Windows con Google Test
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

enable_testing()

include_directories(include)

add_library(project_lib
    src/calculator.cpp
    src/string_utils.cpp
    src/grade_utils.cpp
)

add_executable(run_tests
    tests/test_calculator.cpp
    tests/test_string_utils.cpp
    tests/test_grade_utils.cpp
)

target_link_libraries(run_tests
    project_lib
    gtest
    gtest_main
)

include(GoogleTest)
gtest_discover_tests(run_tests)
