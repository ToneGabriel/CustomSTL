cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)
project(
    CustomSTL
    VERSION 1.0.0
    LANGUAGES C CXX
)

# Languages
set(CMAKE_C_STANDARD            17)
set(CMAKE_C_STANDARD_REQUIRED   ON)
set(CMAKE_C_EXTENSIONS          OFF)

set(CMAKE_CXX_STANDARD          20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)

# Library destinations
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Names
set(TEST_LIBRARY_NAME       "CustomSTLTestLibrary")
set(TEST_EXECUTABLE_NAME    "CustomSTLTest")

# Create test library (test - link executable and other cpp files to this)
add_library(${TEST_LIBRARY_NAME} STATIC)

# CMakeLists subdirectory config
add_subdirectory(config)
add_subdirectory(code_cpp)
add_subdirectory(code_c)

add_subdirectory(test_cpp)
add_subdirectory(test_c)

# CPack settings
set(CPACK_GENERATOR "ZIP")
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
include(CPack)
