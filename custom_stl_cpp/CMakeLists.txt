target_include_directories(${CUSTOM_STL_CPP_LIBRARY} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_sources(${CUSTOM_STL_CPP_FULL_TEST_EXECUTABLE} PRIVATE
    test/full_test_main.cpp
    test/c_thread_test.cpp
    test/c_vector_test.cpp
)
