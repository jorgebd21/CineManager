#include <gtest/gtest.h>

#include "db/database.hpp"
#include "db/repositories/cinerepository.hpp"
#include "db/repositories/pelicularepository.hpp"
#include "db/repositories/salarepository.hpp"
#include "db/repositories/sesionrepository.hpp"

class CinemaReposTest : public ::testing::Test {
 protected:
  SqliteDatabase db{"test_cine.db"};
  CineRepository cineRepo{db};
  PeliculaRepository peliculaRepo{db};
  SalaRepository salaRepo{db};
  SesionRepository sesionRepo{db};

  void SetUp() override {
    // Inicializar esquema de tablas en la base de datos
    (void)SqliteStatement(db.getDb(),
                          "CREATE TABLE IF NOT EXISTS cines ("
                          "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                          "nombre TEXT NOT NULL, direccion TEXT);").step();

    (void)SqliteStatement(db.getDb(),
                          "CREATE TABLE IF NOT EXISTS peliculas ("
                          "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                          "titulo TEXT NOT NULL, genero TEXT NOT NULL, duracion INTEGER NOT NULL);").step();

    (void)SqliteStatement(db.getDb(),
                          "CREATE TABLE IF NOT EXISTS salas ("
                          "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                          "cine_id INTEGER NOT NULL, numero_sala INTEGER NOT NULL, "
                          "filas INTEGER NOT NULL, columnas INTEGER NOT NULL);").step();

    (void)SqliteStatement(db.getDb(),
                          "CREATE TABLE IF NOT EXISTS sesiones ("
                          "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                          "pelicula_id INTEGER NOT NULL, sala_id INTEGER NOT NULL, "
                          "fecha_hora TEXT NOT NULL, precio_entrada REAL NOT NULL);").step();

    // Limpiar tablas
    (void)SqliteStatement(db.getDb(), "DELETE FROM sesiones;").step();
    (void)SqliteStatement(db.getDb(), "DELETE FROM salas;").step();
    (void)SqliteStatement(db.getDb(), "DELETE FROM peliculas;").step();
    (void)SqliteStatement(db.getDb(), "DELETE FROM cines;").step();
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

  obtenida.setTitulo("Inception (Extended)");
  obtenida.setDuracion(155);
  EXPECT_TRUE(peliculaRepo.actualizar(id, obtenida));

  auto todas = peliculaRepo.obtenerTodos();
  EXPECT_EQ(todas.size(), 1u);
  EXPECT_EQ(todas[0].getTitulo(), "Inception (Extended)");
  EXPECT_EQ(todas[0].getDuracion(), 155);
}

TEST_F(CinemaReposTest, SalaCRUD) {
  int idCine = cineRepo.crear(Cine(-1, "Cine Centro", "Calle Gran Vía"));
  Sala sala(-1, idCine, 1, 8, 10);
  int idSala = salaRepo.crear(sala);
  EXPECT_GT(idSala, 0);

  Sala obtenida = salaRepo.obtenerPorId(idSala);
  EXPECT_TRUE(obtenida.esValido());
  EXPECT_EQ(obtenida.getCineId(), idCine);
  EXPECT_EQ(obtenida.getNumeroSala(), 1);
  EXPECT_EQ(obtenida.getFilas(), 8);
  EXPECT_EQ(obtenida.getColumnas(), 10);
  EXPECT_EQ(obtenida.getCapacidad(), 80);

  obtenida.setFilas(10);
  EXPECT_TRUE(salaRepo.actualizar(idSala, obtenida));
  EXPECT_EQ(salaRepo.obtenerPorId(idSala).getCapacidad(), 100);

  auto salasCine = salaRepo.obtenerSalasDeCine(idCine);
  EXPECT_EQ(salasCine.size(), 1u);

  EXPECT_TRUE(salaRepo.eliminar(idSala));
  EXPECT_FALSE(salaRepo.obtenerPorId(idSala).esValido());
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

TEST_F(CinemaReposTest, EntidadesInexistentesDevuelvenInvalido) {
  EXPECT_FALSE(cineRepo.obtenerPorId(99999).esValido());
  EXPECT_FALSE(peliculaRepo.obtenerPorId(99999).esValido());
  EXPECT_FALSE(salaRepo.obtenerPorId(99999).esValido());
  EXPECT_FALSE(sesionRepo.obtenerPorId(99999).esValido());
}
