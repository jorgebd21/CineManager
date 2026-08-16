#include "db/repositories/usuariorepository.hpp"

#include <iostream>
#include "utils/security.hpp"

UsuarioRepository::UsuarioRepository(SqliteDatabase& database) : db(database) {}

bool UsuarioRepository::crear(const Usuario& usuario) {
  try {
    const std::string sql =
        "INSERT INTO usuarios (dni, nombre, apellidos, email, password_hash, "
        "rol) VALUES (?, ?, ?, ?, ?, ?);";
    SqliteStatement stmt(db.getDb(), sql);

    std::string pwd = usuario.getPasswordHash();
    std::string finalHash = (pwd.find('$') != std::string::npos)
                                ? pwd
                                : security::hashPassword(pwd);

    if (!stmt.bindText(1, usuario.getDni()) ||
        !stmt.bindText(2, usuario.getNombre()) ||
        !stmt.bindText(3, usuario.getApellidos()) ||
        !stmt.bindText(4, usuario.getEmail()) ||
        !stmt.bindText(5, finalHash) ||
        !stmt.bindText(6, usuario.getRol())) {
      return false;
    }

    return (stmt.step() == SQLITE_DONE);
  } catch (const std::exception& e) {
    std::cerr << "Error en UsuarioRepository::crear: " << e.what() << std::endl;
    return false;
  }
}

Usuario UsuarioRepository::obtenerPorDni(const std::string& dni) {
  try {
    const std::string sql =
        "SELECT dni, nombre, apellidos, email, password_hash, rol FROM usuarios "
        "WHERE dni = ?;";
    SqliteStatement stmt(db.getDb(), sql);
    if (!stmt.bindText(1, dni)) return Usuario();

    if (stmt.step() == SQLITE_ROW) {
      return Usuario(stmt.getColumnText(0), stmt.getColumnText(1),
                     stmt.getColumnText(2), stmt.getColumnText(3),
                     stmt.getColumnText(4), stmt.getColumnText(5));
    }
  } catch (const std::exception& e) {
    std::cerr << "Error en UsuarioRepository::obtenerPorDni: " << e.what()
              << std::endl;
  }
  return Usuario();
}

Usuario UsuarioRepository::autenticar(const std::string& dni,
                                      const std::string& password) {
  try {
    const std::string sql =
        "SELECT dni, nombre, apellidos, email, password_hash, rol FROM usuarios "
        "WHERE dni = ?;";
    SqliteStatement stmt(db.getDb(), sql);
    if (!stmt.bindText(1, dni)) return Usuario();

    if (stmt.step() == SQLITE_ROW) {
      std::string storedHash = stmt.getColumnText(4);
      if (security::verifyPassword(password, storedHash)) {
        return Usuario(stmt.getColumnText(0), stmt.getColumnText(1),
                       stmt.getColumnText(2), stmt.getColumnText(3),
                       "", stmt.getColumnText(5));
      }
    }
  } catch (const std::exception& e) {
    std::cerr << "Error en UsuarioRepository::autenticar: " << e.what()
              << std::endl;
  }
  return Usuario();
}

bool UsuarioRepository::actualizar(const Usuario& usuario) {
  try {
    const std::string sql =
        "UPDATE usuarios SET nombre = ?, apellidos = ?, email = ?, "
        "password_hash = ?, rol = ? WHERE dni = ?;";
    SqliteStatement stmt(db.getDb(), sql);

    std::string pwd = usuario.getPasswordHash();
    std::string finalHash = (pwd.empty() || pwd.find('$') != std::string::npos)
                                ? pwd
                                : security::hashPassword(pwd);

    if (!stmt.bindText(1, usuario.getNombre()) ||
        !stmt.bindText(2, usuario.getApellidos()) ||
        !stmt.bindText(3, usuario.getEmail()) ||
        !stmt.bindText(4, finalHash) ||
        !stmt.bindText(5, usuario.getRol()) ||
        !stmt.bindText(6, usuario.getDni())) {
      return false;
    }

    return (stmt.step() == SQLITE_DONE);
  } catch (const std::exception& e) {
    std::cerr << "Error en UsuarioRepository::actualizar: " << e.what()
              << std::endl;
    return false;
  }
}

bool UsuarioRepository::eliminar(const std::string& dni) {
  try {
    const std::string sql = "DELETE FROM usuarios WHERE dni = ?;";
    SqliteStatement stmt(db.getDb(), sql);
    if (!stmt.bindText(1, dni)) return false;

    return (stmt.step() == SQLITE_DONE);
  } catch (const std::exception& e) {
    std::cerr << "Error en UsuarioRepository::eliminar: " << e.what()
              << std::endl;
    return false;
  }
}
