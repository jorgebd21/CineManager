# 🎬 CineManager

> 🚧 **Estado del proyecto:** En desarrollo activo (Work in Progress).

**CineManage** es un sistema de gestión y venta de entradas de cine escrito en C++. El objetivo principal de este repositorio no es solo crear una aplicación funcional, sino servir como un **laboratorio de aprendizaje progresivo** de C++. 

El proyecto está diseñado para evolucionar drásticamente en 5 fases: comenzando como una simple aplicación de consola que maneja la memoria directamente mediante Programación Orientada a Objetos (POO), hasta transformarse en una API RESTful multihilo capaz de manejar peticiones concurrentes y persistir datos en una base de datos relacional.

## 🚀 Tecnologías y Herramientas (Planeadas)
- **Lenguaje:** C++ (C++11/C++17)
- **Persistencia:** I/O de Archivos (CSV) ➔ SQLite
- **Concurrencia:** `<thread>`, `<mutex>`
- **Backend Web:** Crow / Drogon (Fase final)

## 🗺️ Roadmap de Desarrollo

Actualmente el proyecto se está desarrollando siguiendo estas fases. Marcaré los avances a medida que se completen:

- [X] **Fase 1: El Motor Básico (Consola y POO)**
  Creación de las entidades principales (Asiento, Sala, Película), lógica de reserva transaccional básica y renderizado visual de butacas en la terminal.
- [X] **Fase 2: Persistencia Básica**
  Implementación de flujos de entrada/salida (I/O) para guardar y cargar el estado de las salas en archivos de texto/CSV, evitando la pérdida de datos al cerrar.
- [ ] **Fase 3: Integración de Base de Datos (SQLite)**
  Sustitución de los archivos de texto por un motor relacional ligero. Migración de la lógica a consultas SQL (SELECT, INSERT, UPDATE).
- [ ] **Fase 4: Concurrencia y Multihilo**
  Refactorización del código para soportar alta demanda simulada. Uso de *mutex* y bloqueos transaccionales para prevenir condiciones de carrera (evitar vender la misma butaca a dos hilos distintos).
- [ ] **Fase 5: Backend y API REST**
  Desacoplamiento de la consola e integración de un framework web (Crow/Drogon) para exponer el sistema mediante endpoints HTTP (JSON), permitiendo que cualquier frontend moderno pueda consumirlo.

## ⚙️ Cómo ejecutar (Temporal)
*(Las instrucciones de compilación mediante `g++` o `CMake` se añadirán aquí una vez que la Fase 1 esté estable y lista para ser probada).*

---
*Nota: Este README es temporal y se actualizará con diagramas de arquitectura, ejemplos de uso de la API y capturas de pantalla una vez que se alcance la versión final.*
