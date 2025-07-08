# get googletest project
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG main
)

# disable googletest installation
set(INSTALL_GTEST OFF CACHE BOOL "Disable installation of googletest" FORCE)

FetchContent_MakeAvailable(googletest)

# get test source files
target_sources(${CUSTOM_STL_CPP_TEST_EXECUTABLE} PRIVATE
    main.cpp
    c_thread_test.cpp
    c_vector_test.cpp
)

# link to executable the following - test library, gtest, gmock
target_link_libraries(${CUSTOM_STL_CPP_TEST_EXECUTABLE} PRIVATE gtest gmock)
