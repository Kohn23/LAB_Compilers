cmake_minimum_required(VERSION 3.15)
project(MiniCompiler VERSION 1.0.0)

set(CMAKE_C_STANDARD 11)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_subdirectory(src)

