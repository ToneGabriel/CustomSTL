
set(CUSTOM_STL_CPP_LIBRARY "Custom_STL_Cpp_Library")
set(CUSTOM_STL_CPP_FULL_TEST_EXECUTABLE "Custom_STL_CPP_FULL_Test")

create_interface_library(${CUSTOM_STL_CPP_LIBRARY} CXX 20 "" "${CMAKE_CURRENT_SOURCE_DIR}/include")

create_executable(${CUSTOM_STL_CPP_FULL_TEST_EXECUTABLE}
    "${CUSTOM_STL_CPP_LIBRARY};gtest;gmock"
    test/main.cpp
    test/custom_thread_test.cpp
    test/custom_vector_test.cpp
    test/custom_unordered_map_test.cpp
    test/custom_unordered_set_test.cpp
)
