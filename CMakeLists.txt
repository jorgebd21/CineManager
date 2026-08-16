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
target_link_libraries(CineManagerCore PUBLIC SQLite::SQLite3)


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

# ==============================================================================
# 3. INTERFAZ GRÁFICA (Capa de Presentación / Qt GUI)
# ==============================================================================
find_package(Qt6 COMPONENTS Widgets REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS "apps/gui/ui")

file(GLOB_RECURSE GUI_SOURCES "apps/gui/src/*.cpp" "apps/gui/include/*.h" "apps/gui/ui/*.ui")

add_executable(CineManagerGUI ${GUI_SOURCES} apps/gui/app/main_gui.cpp)
target_include_directories(CineManagerGUI PRIVATE apps/gui/include)
target_link_libraries(CineManagerGUI PRIVATE CineManagerCore Qt6::Widgets)

# ==============================================================================
# 4. SUITE DE PRUEBAS AUTOMATIZADAS (GoogleTest)
# ==============================================================================
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

file(GLOB_RECURSE TEST_SOURCES "tests/*.cpp")

add_executable(CineManagerTests ${TEST_SOURCES})
target_link_libraries(CineManagerTests PRIVATE CineManagerCore gtest_main)

include(GoogleTest)
gtest_discover_tests(CineManagerTests)