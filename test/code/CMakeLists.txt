# Add sources and include files from current directory to project library
target_include_directories(${TEST_LIBRARY_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_sources(${TEST_LIBRARY_NAME} PUBLIC
    Testing.cpp
    TestingBoost.cpp
)
