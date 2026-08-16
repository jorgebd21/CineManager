#include <gtest/gtest.h>

#include "db/database.hpp"
#include "db/repositories/cinerepository.hpp"
#include "db/repositories/pelicularepository.hpp"
#include "db/repositories/salarepository.hpp"
#include "db/repositories/sesionrepository.hpp"

class CinemaReposTest : public ::testing::Test {
 protected:
  SqliteDatabase db;
  CineRepository cineRepo{db};
  PeliculaRepository peliculaRepo{db};
  SalaRepository salaRepo{db};
  SesionRepository sesionRepo{db};

  void SetUp() override {
    // Inicializar esquema de tablas en la base de datos
    SqliteStatement s1(db.getDb(),
                       "CREATE TABLE IF NOT EXISTS cines ("
                       "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                       "nombre TEXT NOT NULL, direccion TEXT);");
    s1.step();

    SqliteStatement s2(db.getDb(),
                       "CREATE TABLE IF NOT EXISTS peliculas ("
                       "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                       "titulo TEXT NOT NULL, genero TEXT NOT NULL, duracion INTEGER NOT NULL);");
    s2.step();

    SqliteStatement s3(db.getDb(),
                       "CREATE TABLE IF NOT EXISTS salas ("
                       "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                       "cine_id INTEGER NOT NULL, numero_sala INTEGER NOT NULL, "
                       "filas INTEGER NOT NULL, columnas INTEGER NOT NULL);");
    s3.step();

    SqliteStatement s4(db.getDb(),
                       "CREATE TABLE IF NOT EXISTS sesiones ("
                       "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                       "pelicula_id INTEGER NOT NULL, sala_id INTEGER NOT NULL, "
                       "fecha_hora TEXT NOT NULL, precio_entrada REAL NOT NULL);");
    s4.step();

    // Limpiar tablas
    SqliteStatement(db.getDb(), "DELETE FROM sesiones;").step();
    SqliteStatement(db.getDb(), "DELETE FROM salas;").step();
    SqliteStatement(db.getDb(), "DELETE FROM peliculas;").step();
    SqliteStatement(db.getDb(), "DELETE FROM cines;").step();
  }
};

TEST_F(CinemaReposTest, CineCRUD) {
  Cine cine(-1, "Cine Test", "Calle Mayor 1");
  int id = cineRepo.crear(cine);
  EXPECT_GT(id, 0);

  Cine obtenido = cineRepo.obtenerPorId(id);
  EXPECT_TRUE(obtenido.esValido());
  EXPECT_EQ(obtenido.getNombre(), "Cine Test");
  EXPECT_EQ(obtenido.getDireccion(), "Calle Mayor 1");

  obtenido.setNombre("Cine Modificado");
  EXPECT_TRUE(cineRepo.actualizar(id, obtenido));

  Cine modificado = cineRepo.obtenerPorId(id);
  EXPECT_EQ(modificado.getNombre(), "Cine Modificado");

  EXPECT_TRUE(cineRepo.eliminar(id));
  Cine eliminado = cineRepo.obtenerPorId(id);
  EXPECT_FALSE(eliminado.esValido());
}

TEST_F(CinemaReposTest, PeliculaCRUD) {
  Pelicula pelicula(-1, "Inception", Genero::CIENCIA_FICCION, 148);
  int id = peliculaRepo.crear(pelicula);
  EXPECT_GT(id, 0);

  Pelicula obtenida = peliculaRepo.obtenerPorId(id);
  EXPECT_TRUE(obtenida.esValido());
  EXPECT_EQ(obtenida.getTitulo(), "Inception");
  EXPECT_EQ(obtenida.getGenero(), Genero::CIENCIA_FICCION);
  EXPECT_EQ(obtenida.getDuracion(), 148);

  auto todas = peliculaRepo.obtenerTodos();
  EXPECT_EQ(todas.size(), 1u);
}

TEST_F(CinemaReposTest, SesionJoinRelacionalSinNMasUno) {
  int idCine = cineRepo.crear(Cine(-1, "Cine 1", "Direccion 1"));
  int idPelicula = peliculaRepo.crear(Pelicula(-1, "Interstellar", Genero::CIENCIA_FICCION, 169));
  int idSala = salaRepo.crear(Sala(-1, idCine, 1, 5, 5));

  Pelicula peliRef(idPelicula, "Interstellar", Genero::CIENCIA_FICCION, 169);
  std::time_t ahora = 1700000000;
  Sesion sesion(-1, peliRef, idSala, ahora);
  int idSesion = sesionRepo.crear(sesion);
  EXPECT_GT(idSesion, 0);

  // La consulta debe recuperar la película con su título y género en el mismo paso (JOIN)
  Sesion obtenida = sesionRepo.obtenerPorId(idSesion);
  EXPECT_TRUE(obtenida.esValido());
  EXPECT_EQ(obtenida.getIdSala(), idSala);
  EXPECT_EQ(obtenida.getPelicula().getId(), idPelicula);
  EXPECT_EQ(obtenida.getPelicula().getTitulo(), "Interstellar");
  EXPECT_EQ(obtenida.getPelicula().getGenero(), Genero::CIENCIA_FICCION);
  EXPECT_EQ(obtenida.getPelicula().getDuracion(), 169);
}
