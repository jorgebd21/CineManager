# 🎬 CineManager

![C++17](https://img.shields.io/badge/C++-17-blue.svg?style=flat&logo=c%2B%2B)
![SQLite3](https://img.shields.io/badge/SQLite-3-003B57?style=flat&logo=sqlite)
![Arquitectura MVC](https://img.shields.io/badge/Architecture-MVC%20%7C%20Facade-success.svg)
![Build Status](https://img.shields.io/badge/Build-CMake-orange.svg)

> 🚧 **Estado del proyecto:** En desarrollo activo (Work in Progress).

## 📌 Descripción General

**CineManager** es un sistema transaccional de gestión y venta de entradas de cine escrito en C++17. Diseñado originalmente como un **laboratorio de aprendizaje progresivo**, este proyecto evoluciona de manera iterativa hacia una arquitectura desacoplada de nivel comercial que emula los estándares de desarrollo de software crítico y sistemas distribuidos.

El código base se estructura aplicando principios del patrón **MVC (Model-View-Controller)** y **Repository**, aislando la lógica de negocio subyacente y empleando un motor SQLite para la persistencia transaccional (ACID). El sistema permite levantar distintos puntos de acceso separados a nivel de compilación (Administración y Ventas/Cliente) que operan sobre el mismo core.

## 🗺️ Roadmap y Fases de Desarrollo

El proyecto está diseñado bajo un modelo de iteraciones progresivas. Actualmente nos encontramos desarrollando la **Fase 4**:

- [x] **Fase 1: Motor Básico (POO)**
  Lógica principal en memoria, representación en consola y validación estática de reglas de negocio.
- [x] **Fase 2: Persistencia I/O**
  Carga y guardado del estado mediante archivos de texto (CSV) locales.
- [x] **Fase 3: Base de Datos Relacional**
  Integración de SQLite3. Sustitución de los ficheros planos por operaciones CRUD y consultas SQL bajo patrón Repository.
- [X] **Fase 4: Concurrencia y Multihilo**
  Implementación de control de concurrencia y protección ante *race conditions* en entornos multi-cliente usando bloqueos nativos (`std::mutex`) y delegación en transacciones de base de datos.
- [ ] **🚧 Fase 5: Core Library (Arquitectura Hexagonal)**
  Separación total de la lógica de negocio y persistencia en una librería estática/compartida (`libcinemanager_core`).
- [ ] **Fase 6: Interfaz Gráfica Nativa (Qt)**
  Desarrollo de un front-end avanzado utilizando el framework Qt de C++, enlazando directamente con la librería central.
- [ ] **Fase 7: Arquitectura Distribuida (gRPC)**
  Transformación del Core en un servidor independiente, dotando a las interfaces de capacidades de red reales vía gRPC y Protocol Buffers.

## 🛠️ Instrucciones de Compilación y Ejecución

El proyecto puede construirse de manera estándar con CMake o a través del script de automatización incluido, el cual está orientado a mejorar la experiencia de desarrollo (DX) y detectar fugas de memoria.

### Requisitos Previos
*   Compilador C++17 (Clang recomendado para el script, GCC/MSVC soportados por CMake).
*   CMake (3.10 o superior) y [Ninja](https://ninja-build.org/) (opcional, usado por el script).
*   Librería SQLite3 y [Valgrind](https://valgrind.org/) (opcional, para análisis de memoria).

### Opción A: Compilación Rápida (Script Automático)
El script `build.sh` automatiza la configuración con Ninja, compila el código e inicia el programa con herramientas de análisis dinámico.

**1. Ejecución estándar (con AddressSanitizer activo):**
```bash
./build.sh
```

**2. Ejecución bajo auditoría estricta de memoria (con Valgrind):**
```bash
./build.sh --valgrind
```

### Opción B: Compilación Estándar (CMake manual)
Para entornos de producción, Windows, o sistemas que no dispongan de bash/clang:

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

### Ejecución Manual
Los binarios se generan dentro del directorio `build/bin/`.

**Terminal de Administrador:**
```bash
./build/bin/CineManager
```

**Terminal de Cliente / Taquilla:**
```bash
./build/bin/CineManagerClient
```
