@echo off
echo Testing polygon mesh library compilation...

REM Try to compile with g++
echo Attempting to compile with g++...
g++ -std=c++17 -I./include -o test_simple simple_test.cpp 2>nul
if %errorlevel% equ 0 (
    echo SUCCESS: g++ compilation succeeded
    test_simple.exe
    goto :test_lib
) else (
    echo FAILED: g++ not found or compilation failed
)

REM Try to compile with clang++
echo Attempting to compile with clang++...
clang++ -std=c++17 -I./include -o test_simple simple_test.cpp 2>nul
if %errorlevel% equ 0 (
    echo SUCCESS: clang++ compilation succeeded
    test_simple.exe
    goto :test_lib
) else (
    echo FAILED: clang++ not found or compilation failed
)

REM Try with MSVC
echo Attempting to compile with MSVC...
cl /std:c++17 /I.\include simple_test.cpp /Fe:test_simple.exe 2>nul
if %errorlevel% equ 0 (
    echo SUCCESS: MSVC compilation succeeded
    test_simple.exe
    goto :test_lib
) else (
    echo FAILED: MSVC not found or compilation failed
)

echo All compilers failed. Install Visual Studio or MinGW.
goto :end

:test_lib
echo.
echo Testing polygon mesh library...
g++ -std=c++17 -I./include -o test_mesh test_compile.cpp 2>nul
if %errorlevel% equ 0 (
    echo SUCCESS: Library compilation succeeded
    test_mesh.exe
) else (
    clang++ -std=c++17 -I./include -o test_mesh test_compile.cpp 2>nul
    if %errorlevel% equ 0 (
        echo SUCCESS: Library compilation with clang++ succeeded
        test_mesh.exe
    ) else (
        cl /std:c++17 /I.\include test_compile.cpp /Fe:test_mesh.exe 2>nul
        if %errorlevel% equ 0 (
            echo SUCCESS: Library compilation with MSVC succeeded
            test_mesh.exe
        ) else (
            echo FAILED: Library compilation failed with all compilers
        )
    )
)

:end
echo.
echo Build test completed.
pause