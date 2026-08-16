#include <gtest/gtest.h>

#include <algorithm>
#include <set>
#include <vector>

#include "db/database.hpp"
#include "db/repositories/pelicularepository.hpp"
#include "db/repositories/reservarepository.hpp"
#include "db/repositories/usuariorepository.hpp"
#include "models/asiento.hpp"
#include "models/cine.hpp"
#include "models/pelicula.hpp"
#include "models/reserva.hpp"
#include "models/sala.hpp"
#include "models/sesion.hpp"
#include "models/usuario.hpp"

class DomainRulesTest : public ::testing::Test {
 protected:
  const std::string dbPath = "test_domain.db";
  SqliteDatabase db{dbPath};
  UsuarioRepository usuarioRepo{db};
  PeliculaRepository peliculaRepo{db};
  ReservaRepository reservaRepo{db};

  void SetUp() override {
    (void)SqliteStatement(db.getDb(),
                          "CREATE TABLE IF NOT EXISTS usuarios ("
                          "dni TEXT PRIMARY KEY, "
                          "nombre TEXT NOT NULL, "
                          "apellidos TEXT NOT NULL, "
                          "email TEXT NOT NULL UNIQUE, "
                          "password_hash TEXT NOT NULL, "
                          "rol TEXT NOT NULL DEFAULT 'CLIENTE');").step();

    (void)SqliteStatement(db.getDb(),
                          "CREATE TABLE IF NOT EXISTS peliculas ("
                          "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                          "titulo TEXT NOT NULL, "
                          "genero TEXT NOT NULL, "
                          "duracion INTEGER NOT NULL);").step();

    (void)SqliteStatement(db.getDb(),
                          "CREATE TABLE IF NOT EXISTS reservas ("
                          "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                          "sesion_id INTEGER NOT NULL, "
                          "fila INTEGER NOT NULL, "
                          "columna INTEGER NOT NULL, "
                          "estado TEXT NOT NULL DEFAULT 'PENDIENTE', "
                          "timestamp_creacion INTEGER NOT NULL DEFAULT 0, "
                          "tipo TEXT, "
                          "precio REAL, "
                          "UNIQUE(sesion_id, fila, columna));").step();

    // Limpiar tablas
    (void)SqliteStatement(db.getDb(), "DELETE FROM reservas;").step();
    (void)SqliteStatement(db.getDb(), "DELETE FROM peliculas;").step();
    (void)SqliteStatement(db.getDb(), "DELETE FROM usuarios;").step();
  }
};

// 1. Aforos y Sala Llena: Llenado completo de sala y detección de aforo 100%
TEST_F(DomainRulesTest, AforoCompletoYSalaLlena) {
  const int idSesion = 100;
  const int filas = 4;
  const int columnas = 4;
  const int totalButacas = filas * columnas;

  std::vector<Reserva> loteCompleto;
  loteCompleto.reserve(totalButacas);

  for (int f = 0; f < filas; ++f) {
    for (int c = 0; c < columnas; ++c) {
      loteCompleto.emplace_back(-1, idSesion, f, c, "COMPRADO", 1700000000, "Adulto", 7.50f);
    }
  }

  // Insertar todas las butacas
  bool exito = reservaRepo.crearMultiples(loteCompleto);
  EXPECT_TRUE(exito);

  auto reservasActuales = reservaRepo.obtenerPorSesion(idSesion);
  EXPECT_EQ(reservasActuales.size(), static_cast<size_t>(totalButacas));

  // Intentar reservar una butaca extra en una posición ya ocupada debe fallar
  Reserva extra(-1, idSesion, 0, 0, "COMPRADO", 1700000000, "Adulto", 7.50f);
  int idExtra = reservaRepo.crear(extra);
  EXPECT_EQ(idExtra, -1) << "No se deben permitir sobreventas en salas llenas.";
}

// 2. Desglose de Tarifas Mixtas y Precios Límite
TEST_F(DomainRulesTest, DesgloseTarifasMixtasYValoresFrontera) {
  const int idSesion = 200;

  // Insertar reservas con diversas tarifas y valores frontera
  Reserva rAdulto(-1, idSesion, 0, 0, "COMPRADO", 1700000000, "Adulto", 7.50f);
  Reserva rNino(-1, idSesion, 0, 1, "COMPRADO", 1700000000, "Niño", 5.00f);
  Reserva rJubilado(-1, idSesion, 0, 2, "COMPRADO", 1700000000, "Jubilado", 5.50f);
  Reserva rEstudiante(-1, idSesion, 0, 3, "COMPRADO", 1700000000, "Estudiante", 6.00f);
  Reserva rGratis(-1, idSesion, 0, 4, "COMPRADO", 1700000000, "Adulto", 0.00f);  // Tarifa cortesía / 0€

  EXPECT_GT(reservaRepo.crear(rAdulto), 0);
  EXPECT_GT(reservaRepo.crear(rNino), 0);
  EXPECT_GT(reservaRepo.crear(rJubilado), 0);
  EXPECT_GT(reservaRepo.crear(rEstudiante), 0);
  EXPECT_GT(reservaRepo.crear(rGratis), 0);

  auto reservas = reservaRepo.obtenerPorSesion(idSesion);
  EXPECT_EQ(reservas.size(), 5u);

  float recaudacionTotal = 0.0f;
  for (const auto& r : reservas) {
    EXPECT_GE(r.getPrecio(), 0.0f) << "El precio no debe ser negativo.";
    recaudacionTotal += r.getPrecio();
  }

  // Suma esperada: 7.50 + 5.00 + 5.50 + 6.00 + 0.00 = 24.00
  EXPECT_NEAR(recaudacionTotal, 24.00f, 0.001f);
}

// 3. Seguridad e Inmunidad a Inyecciones SQL (SQLi)
TEST_F(DomainRulesTest, InmunidadAInyeccionSQL) {
  // a) Inyección en creación de usuario
  Usuario sqliUser("99999999Z', '1', '1', '1', '1', '1'); DROP TABLE usuarios; --",
                   "Robert'); DROP TABLE usuarios;--",
                   "Smith",
                   "robert@sqli.com",
                   "secret_pass",
                   "CLIENTE");

  EXPECT_TRUE(usuarioRepo.crear(sqliUser));

  // La tabla no fue destruida y los datos se recuperan literalmente
  Usuario recuperado = usuarioRepo.obtenerPorDni("99999999Z', '1', '1', '1', '1', '1'); DROP TABLE usuarios; --");
  EXPECT_TRUE(recuperado.esValido());
  EXPECT_EQ(recuperado.getNombre(), "Robert'); DROP TABLE usuarios;--");

  // b) Inyección en autenticación (intentar bypass con ' OR '1'='1)
  Usuario bypass1 = usuarioRepo.autenticar("' OR '1'='1", "secret_pass");
  EXPECT_FALSE(bypass1.esValido()) << "Autenticación vulnerable a bypass en DNI.";

  Usuario bypass2 = usuarioRepo.autenticar("99999999Z", "' OR '1'='1");
  EXPECT_FALSE(bypass2.esValido()) << "Autenticación vulnerable a bypass en contraseña.";

  // c) Inyección en catálogo de películas
  Pelicula sqliPeli(-1, "Matrix'; DELETE FROM peliculas; --", Genero::CIENCIA_FICCION, 136);
  int idPeli = peliculaRepo.crear(sqliPeli);
  EXPECT_GT(idPeli, 0);

  Pelicula peliRec = peliculaRepo.obtenerPorId(idPeli);
  EXPECT_TRUE(peliRec.esValido());
  EXPECT_EQ(peliRec.getTitulo(), "Matrix'; DELETE FROM peliculas; --");
}

// 4. Validación de Identidad, Roles y Helpers de Dominio
TEST_F(DomainRulesTest, HelpersDominioYModelos) {
  // Test Usuario esValido
  Usuario uVacio;
  EXPECT_FALSE(uVacio.esValido());

  Usuario uInvalidoDni("-1", "Juan", "Perez", "juan@test.com", "pass");
  EXPECT_FALSE(uInvalidoDni.esValido());

  Usuario uValido("12345678A", "Juan", "Perez", "juan@test.com", "pass");
  EXPECT_TRUE(uValido.esValido());

  // Test Roles y conversores
  EXPECT_EQ(rolToString(Rol::ADMIN), "ADMIN");
  EXPECT_EQ(rolToString(Rol::CLIENTE), "CLIENTE");
  EXPECT_EQ(stringToRol("ADMIN"), Rol::ADMIN);
  EXPECT_EQ(stringToRol("admin"), Rol::ADMIN);
  EXPECT_EQ(stringToRol("CLIENTE"), Rol::CLIENTE);
  EXPECT_EQ(stringToRol("cualquier_otra_cosa"), Rol::CLIENTE);

  // Test Asiento C++20 three-way comparison (operator<=>)
  Asiento a1(1, 2);
  Asiento a2(1, 2);
  Asiento a3(1, 3);
  Asiento a4(2, 0);
  Asiento aInvalido(-1, 0);

  EXPECT_TRUE(a1.esValido());
  EXPECT_FALSE(aInvalido.esValido());
  EXPECT_EQ(a1, a2);
  EXPECT_LT(a1, a3);
  EXPECT_LT(a1, a4);

  std::set<Asiento> asientosSet = {a3, a1, a4, a2};
  EXPECT_EQ(asientosSet.size(), 3u);  // a1 y a2 son equivalentes

  // Test Película y Géneros
  EXPECT_EQ(generoToString(Genero::ACCION), "ACCION");
  EXPECT_EQ(generoToString(Genero::CIENCIA_FICCION), "CIENCIA_FICCION");
  EXPECT_EQ(stringToGenero("CIENCIA_FICCION"), Genero::CIENCIA_FICCION);
  EXPECT_EQ(stringToGenero("ACCIÓN"), Genero::ACCION);
  EXPECT_EQ(intToGenero(2), Genero::ACCION);
  EXPECT_EQ(intToGenero(6), Genero::CIENCIA_FICCION);

  // Test Reserva y Estados
  EXPECT_EQ(estadoReservaToString(EstadoReserva::COMPRADO), "COMPRADO");
  EXPECT_EQ(estadoReservaToString(EstadoReserva::PENDIENTE), "PENDIENTE");
  EXPECT_EQ(stringToEstadoReserva("COMPRADO"), EstadoReserva::COMPRADO);
  EXPECT_EQ(stringToEstadoReserva("CONFIRMADA"), EstadoReserva::COMPRADO);
  EXPECT_EQ(stringToEstadoReserva("CANCELADA"), EstadoReserva::CANCELADA);
  EXPECT_EQ(stringToEstadoReserva("PENDIENTE"), EstadoReserva::PENDIENTE);
}
