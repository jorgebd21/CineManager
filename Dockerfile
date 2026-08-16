# ==============================================================================
# STAGE 1: Builder (Compilación y verificación de CineManagerCore, Server y Tests)
# ==============================================================================
FROM ubuntu:22.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    libsqlite3-dev \
    git \
    curl \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY . .

RUN cmake -B build -S . -DCMAKE_BUILD_TYPE=Release && \
    cmake --build build --target CineManagerServer CineManagerTests

# Verificación automatizada de pruebas unitarias durante el proceso de build
RUN ./build/bin/CineManagerTests

# ==============================================================================
# STAGE 2: Runner (Imagen de producción ligera)
# ==============================================================================
FROM ubuntu:22.04 AS runner

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    libsqlite3-0 \
    sqlite3 \
    libasan6 \
    libubsan1 \
    ca-certificates \
    curl \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY --from=builder /app/build/bin/CineManagerServer /app/bin/CineManagerServer
COPY --from=builder /app/data /app/data

EXPOSE 8080

CMD ["/app/bin/CineManagerServer"]
