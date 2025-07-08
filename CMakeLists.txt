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

# Get googletest project
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG main
)

set(INSTALL_GTEST OFF CACHE BOOL "Disable installation of googletest" FORCE)    # disable googletest installation
FetchContent_MakeAvailable(googletest)

# Names
set(CUSTOM_STL_C_LIBRARY "Custom_STL_C_Library")
set(CUSTOM_STL_C_FULL_TEST_EXECUTABLE "Custom_STL_C_Full_Test_Executable")

set(CUSTOM_STL_CPP_LIBRARY "Custom_STL_Cpp_Library")
set(CUSTOM_STL_CPP_FULL_TEST_EXECUTABLE "Custom_STL_Cpp_Full_Test_Executable")

# Interface libraries
add_library(${CUSTOM_STL_C_LIBRARY} INTERFACE)
add_library(${CUSTOM_STL_CPP_LIBRARY} INTERFACE)

# Full Test executables
# add_executable(${CUSTOM_STL_C_FULL_TEST_EXECUTABLE})
add_executable(${CUSTOM_STL_CPP_FULL_TEST_EXECUTABLE})

# Link now, populate later
# target_link_libraries(${CUSTOM_STL_C_FULL_TEST_EXECUTABLE} PRIVATE ${CUSTOM_STL_C_LIBRARY})
target_link_libraries(${CUSTOM_STL_CPP_FULL_TEST_EXECUTABLE} PRIVATE ${CUSTOM_STL_CPP_LIBRARY} gtest gmock)

# CMakeLists subdirectory config
# add_subdirectory(config)
add_subdirectory(custom_stl_c)
add_subdirectory(custom_stl_cpp)


# CPack settings
# set(CPACK_GENERATOR "ZIP")
# set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
# set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
# include(CPack)
