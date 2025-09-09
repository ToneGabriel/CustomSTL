cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)
project(
    CustomSTL
    VERSION 1.0.5
    LANGUAGES C CXX
)

# Get googletest/unity project
include(cmake/googletest.cmake)
include(cmake/unity.cmake)

# Add utils
include(cmake/internal_utils.cmake)

# Configure C and C++ STL
add_subdirectory(custom_stl_c)
add_subdirectory(custom_stl_cpp)
