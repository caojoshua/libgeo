cmake_minimum_required(VERSION 3.27)
project(libgeo)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED True)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")

include_directories(include)
add_subdirectory(src)

# Testing
enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIR})
include(GoogleTest)
add_subdirectory(test)
