# Compiler Principles Lab, Fall 25

## Content
This repo currently contains simplified lexer and parser for MINI Language, which is an made-up programming language.

## How to Run
``script/project.bat`` is all you need, and accepts command-line parameters as follows:
- `config` : set up cmake build system
- `build_lib` : compile minicomp.dll
- `build_test` : compile test.exe
- `run_test` : execute test.exe

The script will run all above if no param is provided.

## Prerequisites
- **Build System**: CMake 4.1.2
- **Compiler**: MinGW-w64 8.1.0
