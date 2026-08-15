# 🎬 CineManager

![C++20](https://img.shields.io/badge/C%2B%2B-17--20-blue.svg?style=flat&logo=c%2B%2B)
![SQLite3](https://img.shields.io/badge/SQLite-3-003B57?style=flat&logo=sqlite)
![Qt6](https://img.shields.io/badge/Qt-6.x-41CD52?style=flat&logo=qt)
![Arquitectura Hexagonal](https://img.shields.io/badge/Architecture-Hexagonal%20%7C%20Repository-success.svg)
![Build Status](https://img.shields.io/badge/Build-CMake-orange.svg)

> 🚧 **Estado del proyecto:** En desarrollo activo (Fase 6 GUI en estado funcional avanzado).

## 📌 Descripción General

**CineManager** es un sistema transaccional de gestión y venta de entradas de cine escrito en C++17/20. Diseñado originalmente como un **laboratorio de aprendizaje progresivo**, este proyecto evoluciona de manera iterativa hacia una arquitectura desacoplada de nivel comercial que emula los estándares de desarrollo de software crítico y sistemas distribuidos.

El código base se estructura aplicando principios del patrón **Arquitectura Hexagonal** y **Repository**, aislando la lógica de negocio subyacente en una librería estática (`CineManagerCore`) y empleando un motor SQLite para la persistencia transaccional (ACID). El sistema permite levantar distintos puntos de acceso separados a nivel de compilación (Consola Administrador, Consola Cliente y GUI Nativa Qt6) que operan sobre el mismo core con control de concurrencia multihilo (`std::mutex` por sesión).

## 🗺️ Roadmap y Fases de Desarrollo

Actualmente nos encontramos refinando la **Fase 6 (Interfaz Gráfica Nativa Qt6)**:

- [x] **Fase 1: Motor Básico (POO)**
  Lógica principal en memoria, representación en consola y validación estática de reglas de negocio.
- [x] **Fase 2: Persistencia I/O**
  Carga y guardado del estado mediante archivos de texto (CSV) locales.
- [x] **Fase 3: Base de Datos Relacional**
  Integración de SQLite3. Sustitución de los ficheros planos por operaciones CRUD y consultas SQL bajo patrón Repository.
- [x] **Fase 4: Concurrencia y Multihilo**
  Implementación de control de concurrencia y protección ante *race conditions* en entornos multi-cliente usando bloqueos fine-grained (`std::mutex` por sesión) y hilo limpiador de reservas expiradas.
- [x] **Fase 5: Core Library (Arquitectura Hexagonal)**
  Separación total de la lógica de negocio y persistencia en una librería estática compartida (`CineManagerCore.a`).
- [x] **Fase 6: Interfaz Gráfica Nativa (Qt6)**
  - [x] Front-end moderno en tema oscuro con tarjetas personalizadas (`CineCardWidget`, `MovieCardWidget`).
  - [x] Filtro dinámico de películas por búsqueda de texto y género en tiempo real.
  - [x] Mapa interactivo de sala con selección múltiple de butacas en `std::set`.
  - [x] **Tarifas y Precios Dinámicos (`TarifasDialog`)**: Selección de tarifas (Adulto, Niño, Jubilado, Estudiante) con cálculo automático en base de datos.
  - [x] **Generación de Código QR Real (`qrcodegen` / `QrHelper`)**: Firma digital de la compra codificada en estándar ISO/IEC 18004 escaneable por móvil.
  - [x] **Gestión y Bloqueo de Salas Llenas (`(LLENA)`)**: Detección en tiempo real de pases agotados y bloqueo visual de botones.
  - [x] Generación e impresión de Ticket de Entrada HTML agrupado con desglose de tarifas por asiento.
- [ ] **Fase 7: Arquitectura Distribuida & API**
  - [ ] Autenticación de Usuarios (Login/Registro con DNI).
  - [ ] Transformación del Core en un servidor HTTP REST (Crow/Drogon) / gRPC.

## 🛠️ Instrucciones de Compilación y Ejecución

El proyecto puede construirse con CMake o mediante el contenedor Docker/DevContainer del proyecto.

### Requisitos Previos
* Compilador C++17/20 (GCC 11+, Clang 12+ o MSVC).
* CMake (3.10 o superior) y Ninja / Make.
* Qt6 (módulo `Widgets`).
* SQLite3 (`libsqlite3-dev`).

### Compilación Rápida (CMake)

```bash
cmake -B build -S .
cmake --build build
```

### Reconstruir Base de Datos de Prueba (Seed Data)
```bash
rm -f data/cine.db && sqlite3 data/cine.db < data/db_init.sql
```

### Ejecución de los Binarios
Los binarios compilados se ubican en `build/bin/`:

**1. GUI Nativa (Recomendada):**
```bash
# Permitir conexiones X11 si estás usando Docker/devcontainer
xhost +

# Ejecución desde terminal local o contenedor
./build/bin/CineManagerGUI
```

**2. Consola de Administrador:**
```bash
./build/bin/CineManager
```

**3. Consola de Cliente:**
```bash
./build/bin/CineManagerClient
```
