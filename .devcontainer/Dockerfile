FROM mcr.microsoft.com/devcontainers/cpp:ubuntu-22.04

# Actualizar e instalar dependencias para Qt6, SQLite3 y soporte gráfico (X11/OpenGL)
RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends \
        qt6-base-dev \
        libgl1-mesa-dev \
        libsqlite3-dev \
        sqlite3 \
        ninja-build \
    && apt-get clean && rm -rf /var/lib/apt/lists/*
