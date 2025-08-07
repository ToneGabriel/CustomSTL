
set(CUSTOM_STL_C_LIBRARY "Custom_STL_C_Library")
set(CUSTOM_STL_C_FULL_TEST_EXECUTABLE "Custom_STL_C_Full_Test_Executable")

create_cxx_library(${CUSTOM_STL_C_LIBRARY} INTERFACE 20 ${CMAKE_CURRENT_SOURCE_DIR}/include)

create_cxx_executable(${CUSTOM_STL_C_FULL_TEST_EXECUTABLE}
    "${CUSTOM_STL_C_LIBRARY};gtest;gmock"
    test/main.cpp
    test/custom_c_priority_queue_test.cpp
    test/custom_c_vector_test.cpp
    test/custom_c_list_test.cpp
    test/custom_c_queue_test.cpp
    test/custom_c_stack_test.cpp
)
