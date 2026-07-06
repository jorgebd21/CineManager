-- Activar el soporte para claves foráneas en SQLite
PRAGMA foreign_keys = ON;

-- Eliminar tablas si existen (útil para reiniciar la base de datos)
DROP TABLE IF EXISTS reservas;
DROP TABLE IF EXISTS sesiones;
DROP TABLE IF EXISTS salas;
DROP TABLE IF EXISTS peliculas;
DROP TABLE IF EXISTS cines;

-- ==========================================
-- 1. Creación de la Estructura de Tablas
-- ==========================================

-- Tabla de Cines
CREATE TABLE cines (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    nombre TEXT NOT NULL,
    direccion TEXT
);

-- Tabla de Películas
CREATE TABLE peliculas (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    titulo TEXT NOT NULL,
    genero TEXT NOT NULL,
    duracion INTEGER NOT NULL -- Duración en minutos
);

-- Tabla de Salas (cada sala pertenece a un cine)
CREATE TABLE salas (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    cine_id INTEGER NOT NULL,
    numero_sala INTEGER NOT NULL,
    filas INTEGER NOT NULL,
    columnas INTEGER NOT NULL,
    FOREIGN KEY (cine_id) REFERENCES cines(id) ON DELETE CASCADE
);

-- Tabla de Sesiones / Funciones
CREATE TABLE sesiones (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    pelicula_id INTEGER NOT NULL,
    sala_id INTEGER NOT NULL,
    fecha_hora TEXT NOT NULL, -- Formato ISO8601: 'YYYY-MM-DD HH:MM:SS'
    precio_entrada REAL NOT NULL,
    FOREIGN KEY (pelicula_id) REFERENCES peliculas(id) ON DELETE CASCADE,
    FOREIGN KEY (sala_id) REFERENCES salas(id) ON DELETE CASCADE
);

-- Tabla de Reservas / Butacas Ocupadas
CREATE TABLE reservas (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    sesion_id INTEGER NOT NULL,
    fila INTEGER NOT NULL,
    columna INTEGER NOT NULL,
    estado TEXT NOT NULL DEFAULT 'RESERVADO', -- 'RESERVADO', 'COMPRADO'
    FOREIGN KEY (sesion_id) REFERENCES sesiones(id) ON DELETE CASCADE,
    UNIQUE(sesion_id, fila, columna) -- Restricción: No se puede duplicar una butaca para la misma sesión
);

-- ==========================================
-- 2. Inserción de Datos Iniciales (Seed Data)
-- ==========================================

-- Insertar Cine por defecto (para nuestra fase de cine único)
INSERT INTO cines (id, nombre, direccion) VALUES 
(1, 'Cine Central Metrópolis', 'Av. de la Constitución 45');

-- Insertar Películas iniciales (tomadas de tus archivos txt)
INSERT INTO peliculas (id, titulo, genero, duracion) VALUES 
(1, 'Avatar 2', 'CIENCIA_FICCION', 192),
(2, 'Pulp Fiction', 'DRAMA', 154);

-- Insertar Salas de prueba para el Cine Central (cine_id: 1)
INSERT INTO salas (id, cine_id, numero_sala, filas, columnas) VALUES 
(1, 1, 1, 5, 7),  -- Sala pequeña: 5 filas x 7 columnas = 35 butacas
(2, 1, 2, 8, 8);  -- Sala mediana: 8 filas x 8 columnas = 64 butacas

-- Insertar Sesiones de prueba para hoy/mañana
INSERT INTO sesiones (id, pelicula_id, sala_id, fecha_hora, precio_entrada) VALUES 
(1, 1, 1, '2026-06-04 18:00:00', 7.50), -- Avatar 2 en Sala 1 a las 18:00
(2, 1, 1, '2026-06-04 21:30:00', 8.00), -- Avatar 2 en Sala 1 a las 21:30
(3, 2, 2, '2026-06-04 20:00:00', 6.00); -- Pulp Fiction en Sala 2 a las 20:00

-- Insertar algunas Reservas iniciales de prueba
INSERT INTO reservas (sesion_id, fila, columna, estado) VALUES 
(1, 2, 3, 'RESERVADO'), -- Fila 2, Columna 3 reservada en la sesión 1
(1, 2, 4, 'RESERVADO'),
(3, 4, 4, 'COMPRADO');  -- Fila 4, Columna 4 comprada en la sesión 3
