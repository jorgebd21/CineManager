-- Activar el soporte para claves foráneas en SQLite
PRAGMA foreign_keys = ON;

-- ==========================================
-- 0. Eliminar tablas si existen (reinicio)
-- ==========================================
DROP TABLE IF EXISTS reservas;
DROP TABLE IF EXISTS sesiones;
DROP TABLE IF EXISTS salas;
DROP TABLE IF EXISTS peliculas;
DROP TABLE IF EXISTS cines;
DROP TABLE IF EXISTS usuarios;

-- ==========================================
-- 1. Creación de la Estructura de Tablas
-- ==========================================

CREATE TABLE usuarios (
    dni TEXT PRIMARY KEY,
    nombre TEXT NOT NULL,
    apellidos TEXT NOT NULL,
    email TEXT NOT NULL UNIQUE,
    password_hash TEXT NOT NULL,
    rol TEXT NOT NULL DEFAULT 'CLIENTE' CHECK (rol IN ('CLIENTE', 'ADMIN'))
);

CREATE TABLE cines (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    nombre TEXT NOT NULL,
    direccion TEXT
);

CREATE TABLE peliculas (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    titulo TEXT NOT NULL,
    genero TEXT NOT NULL,
    duracion INTEGER NOT NULL  -- Duración en minutos
);

CREATE TABLE salas (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    cine_id INTEGER NOT NULL,
    numero_sala INTEGER NOT NULL,
    filas INTEGER NOT NULL,
    columnas INTEGER NOT NULL,
    FOREIGN KEY (cine_id) REFERENCES cines(id) ON DELETE CASCADE
);

CREATE TABLE sesiones (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    pelicula_id INTEGER NOT NULL,
    sala_id INTEGER NOT NULL,
    fecha_hora TEXT NOT NULL,  -- Formato ISO8601: 'YYYY-MM-DD HH:MM:SS'
    precio_entrada REAL NOT NULL,
    FOREIGN KEY (pelicula_id) REFERENCES peliculas(id) ON DELETE CASCADE,
    FOREIGN KEY (sala_id) REFERENCES salas(id) ON DELETE CASCADE
);

CREATE TABLE reservas (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    sesion_id INTEGER NOT NULL,
    fila INTEGER NOT NULL,
    columna INTEGER NOT NULL,
    estado TEXT NOT NULL DEFAULT 'PENDIENTE',  -- 'PENDIENTE', 'COMPRADO'
    timestamp_creacion INTEGER NOT NULL DEFAULT 0,
    tipo TEXT CHECK (tipo IN ('Adulto', 'Niño', 'Jubilado', 'Estudiante')),
    precio REAL,
    FOREIGN KEY (sesion_id) REFERENCES sesiones(id) ON DELETE CASCADE,
    UNIQUE(sesion_id, fila, columna)
);

-- ==========================================
-- 2. Cines
-- ==========================================
INSERT INTO cines (id, nombre, direccion) VALUES
    (1, 'Cine Central Metrópolis',  'Av. de la Constitución 45, Madrid'),
    (2, 'Cine Capitol Premium',     'Gran Vía 12, Planta 3, Madrid');

-- ==========================================
-- 3. Películas (catálogo compartido)
-- ==========================================
INSERT INTO peliculas (id, titulo, genero, duracion) VALUES
    (1,  'Dune: Parte Dos',         'CIENCIA_FICCION', 166),
    (2,  'Oppenheimer',             'DRAMA',           180),
    (3,  'Gladiator II',            'ACCION',          148),
    (4,  'El Señor de los Anillos: La Guerra de los Rohirrim', 'ANIMACION', 134),
    (5,  'Alien: Romulus',          'TERROR',          119),
    (6,  'Deadpool & Wolverine',    'ACCION',          128),
    (7,  'Cónclave',                'DRAMA',           120),
    (8,  'Un Lugar Tranquilo 3',    'TERROR',          103),
    (9,  'Inside Out 2',            'ANIMACION',        96),
    (10, 'Challengers',             'DRAMA',           131);

-- ==========================================
-- 4. Salas
--    Cine 1: 3 salas (pequeña, mediana, grande)
--    Cine 2: 3 salas (pequeña, mediana, VIP)
-- ==========================================
INSERT INTO salas (id, cine_id, numero_sala, filas, columnas) VALUES
    -- Cine Central Metrópolis (cine_id=1)
    (1, 1, 1,  5,  7),  -- Sala 1: 35 butacas (compacta, pases frecuentes)
    (2, 1, 2,  8,  9),  -- Sala 2: 72 butacas (mediana, estrenos)
    (3, 1, 3, 10, 12),  -- Sala 3: 120 butacas (grande, blockbusters)
    -- Cine Capitol Premium (cine_id=2)
    (4, 2, 1,  4,  6),  -- Sala 1: 24 butacas (pequeña, sesiones especiales)
    (5, 2, 2,  7,  8),  -- Sala 2: 56 butacas (mediana, cartelera estándar)
    (6, 2, 3,  6,  6);  -- Sala 3: 36 butacas (formato cuadrado, pases matinales)

-- ==========================================
-- 5. Sesiones
--    Convención de horarios:
--      - Sesión matinal:   +2h desde ahora
--      - Sesión tarde:     +5h
--      - Sesión noche:     +8h
--      - Mañana tarde:     +1 día +5h (~29h)
--      - Mañana noche:     +1 día +8h (~32h)
--      - Pasado mañana:    +2 días +5h
-- ==========================================
INSERT INTO sesiones (id, pelicula_id, sala_id, fecha_hora, precio_entrada) VALUES

    -- ---- CINE 1: Sala 1 (pequeña, 5x7) ----
    -- Dune Parte Dos: 2 pases hoy
    (1,  1, 1, datetime('now', '+2 hours'),          7.50),
    (2,  1, 1, datetime('now', '+5 hours'),          7.50),
    -- Alien Romulus: pase nocturno hoy + mañana tarde
    (3,  5, 1, datetime('now', '+8 hours'),          6.50),
    (4,  5, 1, datetime('now', '+29 hours'),         6.50),

    -- ---- CINE 1: Sala 2 (mediana, 8x9) ----
    -- Oppenheimer: tarde hoy, noche hoy, mañana tarde
    (5,  2, 2, datetime('now', '+3 hours'),          9.00),
    (6,  2, 2, datetime('now', '+7 hours'),          9.00),
    (7,  2, 2, datetime('now', '+31 hours'),         9.00),
    -- Inside Out 2: sesión familiar de tarde
    (8,  9, 2, datetime('now', '+4 hours'),          6.00),
    (9,  9, 2, datetime('now', '+28 hours'),         6.00),

    -- ---- CINE 1: Sala 3 (grande, 10x12) ----
    -- Gladiator II: 3 pases hoy, 2 mañana
    (10, 3, 3, datetime('now', '+1 hour'),           8.50),
    (11, 3, 3, datetime('now', '+4 hours'),          8.50),
    (12, 3, 3, datetime('now', '+8 hours'),          8.50),
    (13, 3, 3, datetime('now', '+25 hours'),         8.50),
    (14, 3, 3, datetime('now', '+30 hours'),         8.50),
    -- Deadpool & Wolverine: tarde hoy + pasado mañana
    (15, 6, 3, datetime('now', '+6 hours'),          8.00),
    (16, 6, 3, datetime('now', '+53 hours'),         8.00),

    -- ---- CINE 2: Sala 4 (pequeña VIP, 4x6) ----
    -- Cónclave: pase único de tarde y noche
    (17, 7, 4, datetime('now', '+5 hours'),         10.00),
    (18, 7, 4, datetime('now', '+9 hours'),         10.00),
    -- Challengers: pase VIP tarde mañana
    (19, 10, 4, datetime('now', '+29 hours'),       10.00),

    -- ---- CINE 2: Sala 5 (mediana, 7x8) ----
    -- Dune Parte Dos: también en Capitol (película compartida)
    (20, 1, 5, datetime('now', '+2 hours'),          8.00),
    (21, 1, 5, datetime('now', '+6 hours'),          8.00),
    -- Un Lugar Tranquilo 3: terror de noche
    (22, 8, 5, datetime('now', '+8 hours'),          7.00),
    (23, 8, 5, datetime('now', '+32 hours'),         7.00),
    -- Inside Out 2: también en Capitol
    (24, 9, 5, datetime('now', '+3 hours'),          6.50),

    -- ---- CINE 2: Sala 6 (cuadrada, 6x6) ----
    -- El Señor de los Anillos Rohirrim: matinal y tarde
    (25, 4, 6, datetime('now', '+2 hours'),          7.50),
    (26, 4, 6, datetime('now', '+5 hours'),          7.50),
    -- Alien Romulus: también en Capitol
    (27, 5, 6, datetime('now', '+7 hours'),          7.00),
    (28, 5, 6, datetime('now', '+55 hours'),         7.00);

-- ==========================================
-- 6. Usuarios de prueba (seed)
-- ==========================================
INSERT INTO usuarios (dni, nombre, apellidos, email, password_hash, rol) VALUES
    ('12345678X', 'Juan',     'Pérez Gómez',    'juan.perez@email.com',   '1234',     'CLIENTE'),
    ('99887766Z', 'María',    'López Ruiz',      'maria.lopez@email.com',  'pass2024',  'CLIENTE'),
    ('11223344A', 'Carlos',   'García Sánchez',  'carlos.garcia@email.com','cinema99', 'CLIENTE'),
    ('87654321Y', 'Admin',    'CineManager',     'admin@cinemanager.com',  'admin123', 'ADMIN');