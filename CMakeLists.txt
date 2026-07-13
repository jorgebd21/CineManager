cmake_minimum_required(VERSION 3.10)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "¡Error! No se permite compilar en la carpeta raíz. Por favor, crea una carpeta 'build' (mkdir build), entra en ella (cd build) y ejecuta 'cmake ..'")
endif()

project(CineManager VERSION 2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(ENABLE_ASAN "Habilitar AddressSanitizer y UndefinedBehaviorSanitizer" ON)
if(ENABLE_ASAN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize=undefined")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
find_package(SQLite3 REQUIRED)

# ==============================================================================
# 1. LIBRERÍA CORE (Capa de Dominio y Datos) - Arquitectura Hexagonal
# ==============================================================================
# Solo compila los archivos de dominio (Base de datos y Modelos)
file(GLOB_RECURSE CORE_SOURCES "core/src/*.cpp")

add_library(CineManagerCore STATIC ${CORE_SOURCES})
# Al poner PUBLIC, cualquier ejecutable que enlace con esta librería heredará automáticamente las cabeceras de 'core/include'
target_include_directories(CineManagerCore PUBLIC core/include)
# Enlazamos SQLite públicamente para que el dominio tenga acceso a la BD
target_link_libraries(CineManagerCore PUBLIC SQLite3::SQLite3)


# ==============================================================================
# 2. ADAPTADORES DE ENTRADA (Capa de Presentación / Consola)
# ==============================================================================
# Compila los controladores y la consola
file(GLOB_RECURSE CONSOLE_SOURCES "apps/console/src/*.cpp")

# -- Ejecutable Administrador --
add_executable(CineManager ${CONSOLE_SOURCES} apps/console/app/main_admin.cpp)
target_include_directories(CineManager PRIVATE apps/console/include)
target_link_libraries(CineManager PRIVATE CineManagerCore)

# -- Ejecutable Cliente --
add_executable(CineManagerClient ${CONSOLE_SOURCES} apps/console/app/main_client.cpp)
target_include_directories(CineManagerClient PRIVATE apps/console/include)
target_link_libraries(CineManagerClient PRIVATE CineManagerCore)