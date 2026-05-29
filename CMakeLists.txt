cmake_minimum_required(VERSION 3.10)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "¡Error! No se permite compilar en la carpeta raíz. Por favor, crea una carpeta 'build' (mkdir build), entra en ella (cd build) y ejecuta 'cmake ..'")
endif()

project(CineManager VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
include_directories(include)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(CineManager ${SOURCES})

target_include_directories(CineManager PRIVATE include)