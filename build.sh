#!/usr/bin/env bash

# Terminar el script inmediatamente si algún comando falla
set -e

# --- CONFIGURACIÓN DE RUTAS ---
BUILD_DIR="build"
EXECUTABLE="./build/bin/CineManager"

# --- DETECCIÓN DE ARGUMENTOS ---
USE_VALGRIND=false
for arg in "$@"; do
    if [ "$arg" == "--valgrind" ]; then
        USE_VALGRIND=true
    fi
done

# --- PASO 1: CONFIGURACIÓN Y GENERACIÓN ---
# Si se pide usar Valgrind, desactivamos ASan. Si no, lo activamos por defecto.
if [ "$USE_VALGRIND" = true ]; then
    echo "========================================================="
    echo " Configurando compilación para Valgrind (ASan desactivado)..."
    echo "========================================================="
    CC=clang CXX=clang++ cmake -G Ninja -B $BUILD_DIR -DCMAKE_BUILD_TYPE=Debug -DENABLE_ASAN=OFF
else
    echo "========================================================="
    echo " Configurando compilación con AddressSanitizer activo..."
    echo "========================================================="
    CC=clang CXX=clang++ cmake -G Ninja -B $BUILD_DIR -DCMAKE_BUILD_TYPE=Debug -DENABLE_ASAN=ON
fi

# --- PASO 2: COMPILACIÓN (NINJA) ---
echo "========================================================="
echo " Compilando el proyecto con Ninja..."
echo "========================================================="
cmake --build $BUILD_DIR

# --- PASO 3: EJECUCIÓN Y ANÁLISIS ---
if [ -f "$EXECUTABLE" ]; then
    if [ "$USE_VALGRIND" = true ]; then
        echo "========================================================="
        echo " Ejecutando binario bajo la auditoría de Valgrind..."
        echo "========================================================="
        valgrind --leak-check=full --track-origins=yes $EXECUTABLE
    else
        echo "========================================================="
        echo " Ejecutando binario directamente (AddressSanitizer activo)..."
        echo "========================================================="
        $EXECUTABLE
    fi
else
    echo "❌ Error: El ejecutable $EXECUTABLE no se encontró."
    exit 1
fi
