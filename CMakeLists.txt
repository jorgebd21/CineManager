cmake_minimum_required(VERSION 3.20)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "¡Error! No se permite compilar en la carpeta raíz. Por favor, crea una carpeta 'build' (mkdir build), entra en ella (cd build) y ejecuta 'cmake ..'")
endif()

project(CineManager VERSION 2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(ENABLE_ASAN "Habilitar AddressSanitizer y UndefinedBehaviorSanitizer" OFF)
if(ENABLE_ASAN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize=undefined")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
find_package(SQLite3 REQUIRED)
find_package(Threads REQUIRED)

if(TARGET SQLite::SQLite3 AND NOT TARGET SQLite3::SQLite3)
    add_library(SQLite3::SQLite3 ALIAS SQLite::SQLite3)
elseif(TARGET SQLite3::SQLite3 AND NOT TARGET SQLite::SQLite3)
    add_library(SQLite::SQLite3 ALIAS SQLite3::SQLite3)
endif()

# ==============================================================================
# 1. LIBRERÍA CORE (Capa de Dominio, Datos y Concurrencia C++20)
# ==============================================================================
set(CORE_SOURCES
    core/src/models/cine.cpp
    core/src/models/pelicula.cpp
    core/src/models/sala.cpp
    core/src/models/sesion.cpp
    core/src/models/reserva.cpp
    core/src/models/usuario.cpp
    core/src/db/database.cpp
    core/src/db/datamanager.cpp
    core/src/db/repositories/cinerepository.cpp
    core/src/db/repositories/pelicularepository.cpp
    core/src/db/repositories/salarepository.cpp
    core/src/db/repositories/sesionrepository.cpp
    core/src/db/repositories/reservarepository.cpp
    core/src/db/repositories/usuariorepository.cpp
    core/src/utils/qrcodegen.cpp
    core/src/utils/security.cpp
)

# Filtramos posibles archivos que existan solo como header
set(CORE_VALID_SOURCES)
foreach(src ${CORE_SOURCES})
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${src}")
        list(APPEND CORE_VALID_SOURCES "${src}")
    endif()
endforeach()

add_library(CineManagerCore STATIC ${CORE_VALID_SOURCES})
target_include_directories(CineManagerCore PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/core/include>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(CineManagerCore PUBLIC SQLite3::SQLite3 Threads::Threads)

# ==============================================================================
# 2. ADAPTADORES DE ENTRADA (Capa de Presentación / Consola CLI)
# ==============================================================================
set(CONSOLE_SOURCES
    apps/console/src/UI/consola.cpp
    apps/console/src/controller/admincontroller.cpp
    apps/console/src/controller/clientcontroller.cpp
)

# -- Ejecutable Administrador --
add_executable(CineManager ${CONSOLE_SOURCES} apps/console/app/main_admin.cpp)
target_include_directories(CineManager PRIVATE apps/console/include)
target_link_libraries(CineManager PRIVATE CineManagerCore)

# -- Ejecutable Cliente --
add_executable(CineManagerClient ${CONSOLE_SOURCES} apps/console/app/main_client.cpp)
target_include_directories(CineManagerClient PRIVATE apps/console/include)
target_link_libraries(CineManagerClient PRIVATE CineManagerCore)

# ==============================================================================
# 3. INTERFAZ GRÁFICA (Capa de Presentación / Qt6 GUI Asíncrona)
# ==============================================================================
find_package(Qt6 COMPONENTS Widgets Network QUIET)

if(Qt6_FOUND)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTOUIC ON)
    set(CMAKE_AUTORCC ON)
    set(CMAKE_AUTOUIC_SEARCH_PATHS "apps/gui/ui")

    set(GUI_SOURCES
        apps/gui/include/apiclient.h
        apps/gui/include/cinecardwidget.h
        apps/gui/include/logindialog.h
        apps/gui/include/mainwindow.h
        apps/gui/include/moviecardwidget.h
        apps/gui/include/qrhelper.h
        apps/gui/include/tarifasdialog.h
        apps/gui/src/apiclient.cpp
        apps/gui/src/cinecardwidget.cpp
        apps/gui/src/moviecardwidget.cpp
        apps/gui/src/qrhelper.cpp
        apps/gui/src/tarifasdialog.cpp
        apps/gui/src/logindialog.cpp
        apps/gui/src/mainwindow.cpp
        apps/gui/ui/logindialog.ui
        apps/gui/ui/mainwindow.ui
        apps/gui/ui/tarifasdialog.ui
        apps/gui/app/main_gui.cpp
    )

    add_executable(CineManagerGUI ${GUI_SOURCES})
    target_include_directories(CineManagerGUI PRIVATE apps/gui/include)
    target_link_libraries(CineManagerGUI PRIVATE CineManagerCore Qt6::Widgets Qt6::Network)
else()
    message(STATUS "Qt6 no detectado: omitiendo la compilación de CineManagerGUI")
endif()

if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

# ==============================================================================
# 4. DEPENDENCIAS EXTERNAS Y SERVIDOR REST (Crow C++ Framework & Asio)
# ==============================================================================
include(FetchContent)

FetchContent_Declare(
    asio
    URL https://github.com/chriskohlhoff/asio/archive/refs/tags/asio-1-30-2.tar.gz
)
FetchContent_GetProperties(asio)
if(NOT asio_POPULATED)
    FetchContent_Populate(asio)
    add_library(Asio INTERFACE)
    target_include_directories(Asio INTERFACE ${asio_SOURCE_DIR}/asio/include)
    target_compile_definitions(Asio INTERFACE ASIO_STANDALONE)
endif()

FetchContent_Declare(
    crow
    URL https://github.com/CrowCpp/Crow/archive/refs/heads/master.zip
)
FetchContent_GetProperties(crow)
if(NOT crow_POPULATED)
    FetchContent_Populate(crow)
    add_library(Crow INTERFACE)
    add_library(Crow::Crow ALIAS Crow)
    target_include_directories(Crow INTERFACE ${crow_SOURCE_DIR}/include)
    target_link_libraries(Crow INTERFACE Asio Threads::Threads)
    target_compile_definitions(Crow INTERFACE ASIO_STANDALONE)
endif()

set(SERVER_SOURCES
    apps/server/src/api_controller.cpp
    apps/server/app/main_server.cpp
)

add_executable(CineManagerServer ${SERVER_SOURCES})
target_include_directories(CineManagerServer PRIVATE apps/server/include)
target_link_libraries(CineManagerServer PRIVATE CineManagerCore Crow::Crow)

# ==============================================================================
# 5. SUITE DE PRUEBAS AUTOMATIZADAS (GoogleTest)
# ==============================================================================
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

set(TEST_SOURCES
    tests/test_main.cpp
    tests/test_usuario_repo.cpp
    tests/test_reserva_repo.cpp
    tests/test_cinema_repos.cpp
    tests/test_concurrency.cpp
    tests/test_expiration_toctou.cpp
    tests/test_database_integrity.cpp
    tests/test_domain_rules.cpp
)

add_executable(CineManagerTests ${TEST_SOURCES})
target_link_libraries(CineManagerTests PRIVATE CineManagerCore gtest_main)

include(GoogleTest)
gtest_discover_tests(CineManagerTests)