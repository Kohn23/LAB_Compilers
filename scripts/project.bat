@echo off
setlocal enabledelayedexpansion
:: params:
:: config

set ACTION=%~1
set BUILD_DIR=build
set BUILD_SUB_DIR=src
set DLL_NAME=libminicomp.dll
set TEST_EXE=test.exe


if "%ACTION%"=="" (
    echo [1/4] config...
    call :config
    if errorlevel 1 (
        echo config FAIL
        exit /b 1
    )
    
    echo.
    echo [2/4] build_lib...
    call :build_lib
    if errorlevel 1 (
        echo build_lib FAIL
        exit /b 1
    )
    
    echo.
    echo [3/4] build_test...
    call :build_test
    if errorlevel 1 (
        echo build_test FAIL
        exit /b 1
    )
    
    echo.
    echo [4/4] run...
    call :run
    if errorlevel 1 (
        echo run FAIL
        exit /b 1
    )
    
    echo.
    echo COMPLETE ALL
    exit /b 0
)

if "%ACTION%"=="config" (
:config
    echo === Config Cmake ===
    if not exist %BUILD_DIR% mkdir %BUILD_DIR%
    cd %BUILD_DIR%
    cmake .. -G "MinGW Makefiles" -DCMAKE_TOOLCHAIN_FILE="cmake/mingw-toolchain.cmake" -DCMAKE_MAKE_PROGRAM=C:\Users\Kohn\GnuWin32\bin\make.exe

    cd ..
    echo complete
    exit /b 0
)

if "%ACTION%"=="build_lib" (
:build_lib
    echo === Compile DLL ===
    cd %BUILD_DIR%
    cmake --build .
    cd ..
    echo complete
    exit /b 0
)

if "%ACTION%"=="build_test" (
:build_test
    if not exist %BUILD_DIR%\%BUILD_SUB_DIR%\%DLL_NAME% (
    echo === Build Test ===
        echo Not Found: %BUILD_DIR%\%BUILD_SUB_DIR%\%DLL_NAME%
        echo plz run: %0 build_lib
        exit /b 1
    )

    cd test
    gcc main.cpp -L../%BUILD_DIR%/%BUILD_SUB_DIR% -lmincomp -o ../%BUILD_DIR%/%BUILD_SUB_DIR% /%TEST_EXE%
    cd ..
    echo complete: %BUILD_DIR%\%BUILD_SUB_DIR%\%TEST_EXE%
    exit /b 0
)

if "%ACTION%"=="run" (
:run
    echo === run test ===
    if not exist %BUILD_DIR%\%TEST_EXE% (
        echo Not Found: %BUILD_DIR%\%TEST_EXE%
        echo plz run: %0 build_test
        exit /b 1
    )

    cd %BUILD_DIR%
    .\%TEST_EXE%
    cd ..
    exit /b 0
)

echo Unknown cmd: %ACTION%
echo Valid cmd: config, build_lib, build_test, run
exit /b 1
