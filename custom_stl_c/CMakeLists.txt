target_include_directories(${CUSTOM_STL_C_LIBRARY} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_sources(${CUSTOM_STL_C_FULL_TEST_EXECUTABLE} PRIVATE
    test/full_test_main.cpp
    test/c_priority_queue_test.cpp
)
