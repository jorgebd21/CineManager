#ifndef USUARIOREPOSITORY_HPP
#define USUARIOREPOSITORY_HPP

#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "db/database.hpp"
#include "models/usuario.hpp"

class IUsuarioRepository {
 public:
  virtual ~IUsuarioRepository() = default;
  virtual bool crear(const Usuario& usuario) = 0;
  virtual Usuario obtenerPorDni(const std::string& dni) = 0;
  virtual Usuario autenticar(const std::string& dni, const std::string& password) = 0;
  virtual bool actualizar(const Usuario& usuario) = 0;
  virtual bool eliminar(const std::string& dni) = 0;
};

class UsuarioRepository : public IUsuarioRepository {
 private:
  SqliteDatabase& db;

 public:
  explicit UsuarioRepository(SqliteDatabase& database);

  bool crear(const Usuario& usuario) override;
  Usuario obtenerPorDni(const std::string& dni) override;
  Usuario autenticar(const std::string& dni, const std::string& password) override;
  bool actualizar(const Usuario& usuario) override;
  bool eliminar(const std::string& dni) override;
};

#endif  // USUARIOREPOSITORY_HPP