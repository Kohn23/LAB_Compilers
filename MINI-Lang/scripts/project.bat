@echo off
setlocal enabledelayedexpansion
:: params:
:: config

set ACTION=%~1
set BUILD_DIR=build
set BUILD_LIB_DIR=src
set BUILD_TEST_DIR=test
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
    echo [4/4] run_test...
    call :run_test
    if errorlevel 1 (
        echo run_test FAIL
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
    if not exist %BUILD_TEST_DIR% mkdir %BUILD_TEST_DIR%
    copy %BUILD_LIB_DIR%\%DLL_NAME% %BUILD_TEST_DIR%\

    cd ..
    echo complete
    exit /b 0
)

if "%ACTION%"=="build_test" (
:build_test
    if not exist %BUILD_DIR%\%BUILD_LIB_DIR%\%DLL_NAME% (
    echo === Build Test ===
        echo Not Found: %BUILD_DIR%\%BUILD_LIB_DIR%\%DLL_NAME%
        echo plz run: %0 build_lib
        exit /b 1
    )
    

    cd test
    gcc LabTest.c -L../%BUILD_DIR%/%BUILD_LIB_DIR% -lminicomp -o ../%BUILD_DIR%/%BUILD_TEST_DIR%/%TEST_EXE%
    cd ..
    echo complete: %BUILD_DIR%\%BUILD_TEST_DIR%\%TEST_EXE%
    exit /b 0
)

if "%ACTION%"=="run_test" (
:run_test
    echo === run test ===
    if not exist %BUILD_DIR%\%BUILD_TEST_DIR%\%TEST_EXE% (
        echo Not Found: %BUILD_DIR%\%BUILD_TEST_DIR%\%TEST_EXE%
        echo plz run: %0 build_test
        exit /b 1
    )

    .\%BUILD_DIR%\%BUILD_TEST_DIR%\%TEST_EXE%
    exit /b 0
)

echo Unknown cmd: %ACTION%
echo Valid cmd: config, build_lib, build_test, run_test
exit /b 1
