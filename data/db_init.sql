-- Activar el soporte para claves foráneas en SQLite
PRAGMA foreign_keys = ON;
-- Eliminar tablas si existen (útil para reiniciar la base de datos)
DROP TABLE IF EXISTS reservas;
DROP TABLE IF EXISTS sesiones;
DROP TABLE IF EXISTS salas;
DROP TABLE IF EXISTS peliculas;
DROP TABLE IF EXISTS cines;
DROP TABLE IF EXISTS usuarios;

-- ==========================================
-- 1. Creación de la Estructura de Tablas
-- ==========================================
-- Tabla de Usuarios
CREATE TABLE usuarios (
    dni TEXT PRIMARY KEY,
    nombre TEXT NOT NULL,
    apellidos TEXT NOT NULL,
    email TEXT NOT NULL UNIQUE,
    password_hash TEXT NOT NULL,
    rol TEXT NOT NULL DEFAULT 'CLIENTE' CHECK (rol IN ('CLIENTE', 'ADMIN'))
);

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
    fecha_hora TEXT NOT NULL,
    -- Formato ISO8601: 'YYYY-MM-DD HH:MM:SS'
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
    estado TEXT NOT NULL DEFAULT 'PENDIENTE',
    -- 'PENDIENTE', 'COMPRADO'
    timestamp_creacion INTEGER NOT NULL DEFAULT 0,
    -- Unix timestamp de creación
    tipo TEXT CHECK (
        tipo IN ('Adulto', 'Niño', 'Jubilado', 'Estudiante')
    ),
    precio REAL,
    FOREIGN KEY (sesion_id) REFERENCES sesiones(id) ON DELETE CASCADE,
    UNIQUE(sesion_id, fila, columna) -- Restricción: No se puede duplicar una butaca para la misma sesión
);
-- ==========================================
-- 2. Inserción de Datos Iniciales (Seed Data)
-- ==========================================
-- Insertar Cine por defecto (para nuestra fase de cine único)
INSERT INTO cines (id, nombre, direccion)
VALUES (
        1,
        'Cine Central Metrópolis',
        'Av. de la Constitución 45'
    ),
    (
        2,
        'Cine Capitol Premium',
        'Gran Vía 12, Planta 3'
    );
-- Insertar Películas iniciales (tomadas de tus archivos txt)
INSERT INTO peliculas (id, titulo, genero, duracion)
VALUES (1, 'Avatar 2', 'CIENCIA_FICCION', 192),
    (2, 'Pulp Fiction', 'DRAMA', 154),
    (3, 'Gladiator 2', 'ACCION', 150),
    (4, 'Interstellar', 'CIENCIA_FICCION', 169),
    (5, 'The Dark Knight', 'ACCION', 152),
    (6, 'The Hangover', 'COMEDIA', 100),
    (7, 'The Conjuring', 'TERROR', 112),
    (8, 'La La Land', 'ROMANCE', 128);
-- Insertar Salas de prueba para el Cine Central (cine_id: 1) y Cine Capitol (cine_id: 2)
INSERT INTO salas (id, cine_id, numero_sala, filas, columnas)
VALUES (1, 1, 1, 5, 7),
    -- Sala pequeña: 5 filas x 7 columnas = 35 butacas (Cine 1)
    (2, 1, 2, 8, 8),
    -- Sala mediana: 8 filas x 8 columnas = 64 butacas (Cine 1)
    (3, 2, 1, 6, 6),
    -- Sala mediana: 6 filas x 6 columnas = 36 butacas (Cine 2)
    (4, 2, 2, 4, 5),
    -- Sala pequeña: 4 filas x 5 columnas = 20 butacas (Cine 2)
    -- SALAS CON VALORES ABSURDOS DE PRUEBA (Cine 1)
    (5, 1, 3, 20, 20),
    -- Sala gigante: 20x20 = 400 butacas (Cine 1, Sala 3)
    (6, 1, 4, 2, 2),
    -- Sala minúscula (Cápsula VIP): 2x2 = 4 butacas (Cine 1, Sala 4)
    (7, 1, 5, 3, 15),
    -- Sala extra-ancha: 3x15 = 45 butacas (Cine 1, Sala 5)
    (8, 1, 6, 12, 3);
-- Sala extra-estrecha (pasillo): 12x3 = 36 butacas (Cine 1, Sala 6)
-- Insertar Sesiones de prueba para hoy/mañana
INSERT INTO sesiones (
        id,
        pelicula_id,
        sala_id,
        fecha_hora,
        precio_entrada
    )
VALUES -- Cine 1 (Sala 1 y 2)
    (1, 1, 1, datetime('now', '+1 hour'), 7.50),
    -- Avatar 2 en Sala 1 (Empieza en 1h)
    (2, 1, 1, datetime('now', '+3 hours'), 8.00),
    -- Avatar 2 en Sala 1 (Empieza en 3h)
    (3, 2, 2, datetime('now', '+2 hours'), 6.00),
    -- Pulp Fiction en Sala 2 (Empieza en 2h)
    (4, 3, 2, datetime('now', '+4 hours'), 7.00),
    -- Gladiator 2 en Sala 2 (Empieza en 4h)
    (5, 4, 1, datetime('now', '-1 hour'), 6.50),
    -- Interstellar en Sala 1 (CADUCADA, hace 1h)
    (12, 1, 1, datetime('now', '+1 day'), 7.50),
    -- Avatar 2 en Sala 1 (Mañana)
    (13, 1, 1, datetime('now', '+27 hours'), 8.00),
    -- Avatar 2 en Sala 1 (Mañana + 3h)
    (14, 1, 2, datetime('now', '+2 days'), 8.00),
    -- Avatar 2 en Sala 2 (Pasado mañana)
    (15, 1, 2, datetime('now', '+52 hours'), 8.50),
    -- Avatar 2 en Sala 2 (Pasado mañana + 4h)
    (16, 1, 1, datetime('now', '+3 days'), 6.50),
    -- Avatar 2 en Sala 1 (En 3 días)
    (17, 1, 1, datetime('now', '+77 hours'), 7.50),
    -- Avatar 2 en Sala 1 (En 3 días + 5h)
    (22, 1, 1, datetime('now', '-3 hours'), 7.50),
    -- Avatar 2 en Sala 1 (CADUCADA, hace 3h para pruebas de criba)
    -- Sesiones asociadas a las salas absurdas (Avatar 2, Cine 1)
    (18, 1, 5, datetime('now', '+30 hours'), 9.00),
    -- Avatar 2 en Sala Gigante (20x20)
    (19, 1, 6, datetime('now', '+50 hours'), 12.00),
    -- Avatar 2 en Sala VIP (2x2)
    (20, 1, 7, datetime('now', '+73 hours'), 7.50),
    -- Avatar 2 en Sala Ancha (3x15)
    (21, 1, 8, datetime('now', '+78 hours'), 7.00),
    -- Avatar 2 en Sala Pasillo (12x3)
    -- Cine 2 (Sala 3 y 4)
    (6, 2, 3, datetime('now', '+3 hours'), 6.00),
    -- Pulp Fiction en Sala 3
    (7, 3, 3, datetime('now', '+5 hours'), 7.00),
    -- Gladiator 2 en Sala 3
    (8, 5, 4, datetime('now', '+2 hours'), 7.50),
    -- The Dark Knight en Sala 4
    (9, 6, 4, datetime('now', '+4 hours'), 5.50),
    -- The Hangover en Sala 4
    (10, 7, 3, datetime('now', '+6 hours'), 6.00),
    -- The Conjuring en Sala 3
    (11, 8, 4, datetime('now', '-2 hours'), 6.50);
-- La La Land en Sala 4 (CADUCADA, hace 2h)

-- Insertar Usuarios de prueba (Semilla)
INSERT INTO usuarios (dni, nombre, apellidos, email, password_hash, rol)
VALUES 
    ('12345678X', 'Juan', 'Pérez Gómez', 'juan.perez@email.com', '1234', 'CLIENTE'),
    ('87654321Y', 'Admin', 'CineManager', 'admin@cinemanager.com', 'admin123', 'ADMIN');