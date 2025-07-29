cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)
project(
    CustomSTL
    VERSION 1.0.0
    LANGUAGES C CXX
)

# Add utils
include(cmake/internal_utils.cmake)

# Get googletest project
include(cmake/fetch_googletest.cmake)

# Names
set(CUSTOM_STL_C_LIBRARY "Custom_STL_C_Library")
set(CUSTOM_STL_C_FULL_TEST_EXECUTABLE "Custom_STL_C_Full_Test_Executable")

set(CUSTOM_STL_CPP_LIBRARY "Custom_STL_Cpp_Library")
set(CUSTOM_STL_CPP_FULL_TEST_EXECUTABLE "Custom_STL_Cpp_Full_Test_Executable")

create_cxx_library(${CUSTOM_STL_CPP_LIBRARY} INTERFACE 20)
create_cxx_executable(${CUSTOM_STL_CPP_FULL_TEST_EXECUTABLE} ${CUSTOM_STL_CPP_LIBRARY} gtest gmock)

# Interface libraries
# add_library(${CUSTOM_STL_C_LIBRARY} INTERFACE)
# add_library(${CUSTOM_STL_CPP_LIBRARY} INTERFACE)

# # Full Test executables
# add_executable(${CUSTOM_STL_C_FULL_TEST_EXECUTABLE})
# add_executable(${CUSTOM_STL_CPP_FULL_TEST_EXECUTABLE})

# # Link now, populate later
# target_link_libraries(${CUSTOM_STL_C_FULL_TEST_EXECUTABLE} PRIVATE ${CUSTOM_STL_C_LIBRARY} gtest gmock)
# target_link_libraries(${CUSTOM_STL_CPP_FULL_TEST_EXECUTABLE} PRIVATE ${CUSTOM_STL_CPP_LIBRARY} gtest gmock)

# CMakeLists subdirectory config
# add_subdirectory(config)
# add_subdirectory(custom_stl_c)
add_subdirectory(custom_stl_cpp)


# CPack settings
# set(CPACK_GENERATOR "ZIP")
# set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
# set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
# include(CPack)
