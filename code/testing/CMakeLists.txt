# Add sources and include files from current directory to project library
target_include_directories(${PROJECT_LIBRARY_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_sources(${PROJECT_LIBRARY_NAME} PUBLIC
    Testing.cpp
    TestingBoost.cpp
)
