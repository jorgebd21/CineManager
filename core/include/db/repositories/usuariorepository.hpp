#ifndef USUARIOREPOSITORY_HPP
#define USUARIOREPOSITORY_HPP

#include <vector>

#include "db/database.hpp"
#include "models/usuario.hpp"

class UsuarioRepository {
 private:
  SqliteDatabase& db;

 public:
  explicit UsuarioRepository(SqliteDatabase& database);

  bool crear(const Usuario& usuario);
  Usuario obtenerPorDni(const std::string& dni);
  Usuario autenticar(const std::string& dni, const std::string& password);
  bool actualizar(const Usuario& usuario);
  bool eliminar(const std::string& dni);
};

#endif  // USUARIOREPOSITORY_HPP