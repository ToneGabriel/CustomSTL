@echo off
echo Running all test...

echo.
echo Building tests...
echo.
cmake -G "Ninja" -B build
cmake --build build


cd build/bin

echo.
echo Starting CPP Tests with Googletest...
echo.
Custom_STL_CPP_FULL_Test.exe

echo.
echo Starting C Tests with Unity...
echo.
Custom_STL_C_LIST_Test.exe
Custom_STL_C_PRIORITY_QUEUE_Test.exe
Custom_STL_C_QUEUE_Test.exe
Custom_STL_C_STACK_Test.exe
Custom_STL_C_VECTOR_Test.exe

echo.
echo All tests finished.
pause
