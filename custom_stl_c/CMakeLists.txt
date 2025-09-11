
set(CUSTOM_STL_C_LIBRARY "Custom_STL_C_Library")
create_interface_library(${CUSTOM_STL_C_LIBRARY} C 99 "" "${CMAKE_CURRENT_SOURCE_DIR}/include")

# ====================================================================================
set(CUSTOM_STL_C_VECTOR_TEST_EXECUTABLE "Custom_STL_C_VECTOR_Test")
create_executable(${CUSTOM_STL_C_VECTOR_TEST_EXECUTABLE}
    "${CUSTOM_STL_C_LIBRARY};unity"
    test/custom_c_vector_test.c
)

# ====================================================================================
set(CUSTOM_STL_C_LIST_TEST_EXECUTABLE "Custom_STL_C_LIST_Test")
create_executable(${CUSTOM_STL_C_LIST_TEST_EXECUTABLE}
    "${CUSTOM_STL_C_LIBRARY};unity"
    test/custom_c_list_test.c
)

# ====================================================================================
set(CUSTOM_STL_C_PRIORITY_QUEUE_TEST_EXECUTABLE "Custom_STL_C_PRIORITY_QUEUE_Test")
create_executable(${CUSTOM_STL_C_PRIORITY_QUEUE_TEST_EXECUTABLE}
    "${CUSTOM_STL_C_LIBRARY};unity"
    test/custom_c_priority_queue_test.c
)

# ====================================================================================
set(CUSTOM_STL_C_QUEUE_TEST_EXECUTABLE "Custom_STL_C_QUEUE_Test")
create_executable(${CUSTOM_STL_C_QUEUE_TEST_EXECUTABLE}
    "${CUSTOM_STL_C_LIBRARY};unity"
    test/custom_c_queue_test.c
)

# ====================================================================================
set(CUSTOM_STL_C_STACK_TEST_EXECUTABLE "Custom_STL_C_STACK_Test")
create_executable(${CUSTOM_STL_C_STACK_TEST_EXECUTABLE}
    "${CUSTOM_STL_C_LIBRARY};unity"
    test/custom_c_stack_test.c
)

# ====================================================================================
create_ctest_suite(
    ${CUSTOM_STL_C_VECTOR_TEST_EXECUTABLE}
    ${CUSTOM_STL_C_LIST_TEST_EXECUTABLE}
    ${CUSTOM_STL_C_PRIORITY_QUEUE_TEST_EXECUTABLE}
    ${CUSTOM_STL_C_QUEUE_TEST_EXECUTABLE}
    ${CUSTOM_STL_C_STACK_TEST_EXECUTABLE}
)
