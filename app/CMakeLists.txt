# Crete executable and link it to the library
add_executable(${PROJECT_EXECUTABLE_NAME} main.cpp)
target_link_libraries(${PROJECT_EXECUTABLE_NAME} PUBLIC ${PROJECT_LIBRARY_NAME})
