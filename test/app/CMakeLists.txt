# Crete executable and link it to the library
add_executable(${TEST_EXECUTABLE_NAME} test_main.cpp)
target_link_libraries(${TEST_EXECUTABLE_NAME} PUBLIC ${TEST_LIBRARY_NAME} gtest)    # gtest_main
