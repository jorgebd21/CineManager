cmake_minimum_required(VERSION 3.10)

project(CineManager VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(CineManager ${SOURCES})

target_include_directories(CineManager PRIVATE include)