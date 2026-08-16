#include <gtest/gtest.h>

#include "db/database.hpp"
#include "db/repositories/usuariorepository.hpp"
#include "models/usuario.hpp"

class UsuarioRepoTest : public ::testing::Test {
 protected:
  SqliteDatabase db{"test_cine.db"};
  UsuarioRepository repo{db};

  void SetUp() override {
    SqliteStatement stmt(db.getDb(),
                         "CREATE TABLE IF NOT EXISTS usuarios ("
                         "dni TEXT PRIMARY KEY, "
                         "nombre TEXT NOT NULL, "
                         "apellidos TEXT NOT NULL, "
                         "email TEXT NOT NULL UNIQUE, "
                         "password_hash TEXT NOT NULL, "
                         "rol TEXT NOT NULL DEFAULT 'CLIENTE');");
    (void)stmt.step();

    // Limpiar tabla antes de cada test
    SqliteStatement clearStmt(db.getDb(), "DELETE FROM usuarios;");
    (void)clearStmt.step();
  }
};

TEST_F(UsuarioRepoTest, EstadoValidoHelper) {
  Usuario valido("12345678X", "Juan", "Pérez", "juan@email.com", "1234", "CLIENTE");
  Usuario invalido;

  EXPECT_TRUE(valido.esValido());
  EXPECT_FALSE(invalido.esValido());
}

TEST_F(UsuarioRepoTest, CrearYObtenerUsuario) {
  Usuario user("11111111A", "Ana", "López", "ana@email.com", "pass123", "CLIENTE");
  EXPECT_TRUE(repo.crear(user));

  Usuario recuperado = repo.obtenerPorDni("11111111A");
  EXPECT_TRUE(recuperado.esValido());
  EXPECT_EQ(recuperado.getDni(), "11111111A");
  EXPECT_EQ(recuperado.getNombre(), "Ana");
  EXPECT_EQ(recuperado.getApellidos(), "López");
  EXPECT_EQ(recuperado.getEmail(), "ana@email.com");
  EXPECT_EQ(recuperado.getRol(), "CLIENTE");
}

TEST_F(UsuarioRepoTest, AutenticarCredenciales) {
  Usuario user("22222222B", "Carlos", "Sánchez", "carlos@email.com", "mi_pass_secreta", "ADMIN");
  ASSERT_TRUE(repo.crear(user));

  // Autenticación correcta
  Usuario authOk = repo.autenticar("22222222B", "mi_pass_secreta");
  EXPECT_TRUE(authOk.esValido());
  EXPECT_EQ(authOk.getNombre(), "Carlos");
  EXPECT_EQ(authOk.getRol(), "ADMIN");

  // Autenticación con contraseña errónea
  Usuario authFailPass = repo.autenticar("22222222B", "password_incorrecta");
  EXPECT_FALSE(authFailPass.esValido());

  // Autenticación con DNI inexistente
  Usuario authFailDni = repo.autenticar("99999999Z", "mi_pass_secreta");
  EXPECT_FALSE(authFailDni.esValido());
}

TEST_F(UsuarioRepoTest, RestriccionesDeClavePrimariaYEmailUnico) {
  Usuario user1("33333333C", "Laura", "Gómez", "laura@email.com", "pass1", "CLIENTE");
  EXPECT_TRUE(repo.crear(user1));

  // Duplicación de DNI (Primary Key)
  Usuario userDniDuplicado("33333333C", "Otra", "Persona", "otra@email.com", "pass2", "CLIENTE");
  EXPECT_FALSE(repo.crear(userDniDuplicado)) << "No debe permitir duplicar DNI.";

  // Duplicación de Email (UNIQUE constraint)
  Usuario userEmailDuplicado("44444444D", "Tercero", "Persona", "laura@email.com", "pass3", "CLIENTE");
  EXPECT_FALSE(repo.crear(userEmailDuplicado)) << "No debe permitir duplicar email.";
}

TEST_F(UsuarioRepoTest, ActualizarYEliminarUsuario) {
  Usuario user("55555555E", "Elena", "Martín", "elena@email.com", "1234", "CLIENTE");
  ASSERT_TRUE(repo.crear(user));

  // Modificar datos
  user.setNombre("Elena María");
  user.setEmail("elena.maria@email.com");
  EXPECT_TRUE(repo.actualizar(user));

  Usuario actualizado = repo.obtenerPorDni("55555555E");
  EXPECT_EQ(actualizado.getNombre(), "Elena María");
  EXPECT_EQ(actualizado.getEmail(), "elena.maria@email.com");

  // Eliminar
  EXPECT_TRUE(repo.eliminar("55555555E"));
  Usuario eliminado = repo.obtenerPorDni("55555555E");
  EXPECT_FALSE(eliminado.esValido());
}

TEST_F(UsuarioRepoTest, SoporteCaracteresEspecialesYUTF8) {
  Usuario user("66666666F", "Íñigo", "Núñez de la Peña", "inigo.nunez@ñandú.es", "p@$$w0rd!#%&/", "CLIENTE");
  EXPECT_TRUE(repo.crear(user));

  Usuario rec = repo.obtenerPorDni("66666666F");
  EXPECT_TRUE(rec.esValido());
  EXPECT_EQ(rec.getNombre(), "Íñigo");
  EXPECT_EQ(rec.getApellidos(), "Núñez de la Peña");
  EXPECT_EQ(rec.getEmail(), "inigo.nunez@ñandú.es");
}
