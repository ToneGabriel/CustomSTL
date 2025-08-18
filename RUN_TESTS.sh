#!/bin/sh
echo "Running all tests..."

echo
echo Building tests...
echo
cmake -G "Ninja" -B build
cmake --build build


cd build/bin

echo
echo "Starting CPP Tests with Googletest..."
echo
./Custom_STL_CPP_FULL_Test

echo
echo "Starting C Tests with Unity..."
echo
./Custom_STL_C_LIST_Test
./Custom_STL_C_PRIORITY_QUEUE_Test
./Custom_STL_C_QUEUE_Test
./Custom_STL_C_STACK_Test
./Custom_STL_C_VECTOR_Test

echo
echo "All tests finished."
