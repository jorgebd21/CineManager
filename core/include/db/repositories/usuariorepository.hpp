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
  [[nodiscard]] virtual bool crear(const Usuario& usuario) = 0;
  [[nodiscard]] virtual Usuario obtenerPorDni(const std::string& dni) = 0;
  [[nodiscard]] virtual Usuario autenticar(const std::string& dni, const std::string& password) = 0;
  [[nodiscard]] virtual bool actualizar(const Usuario& usuario) = 0;
  [[nodiscard]] virtual bool eliminar(const std::string& dni) = 0;
};

class UsuarioRepository : public IUsuarioRepository {
 private:
  SqliteDatabase& db;

 public:
  explicit UsuarioRepository(SqliteDatabase& database);

  [[nodiscard]] bool crear(const Usuario& usuario) override;
  [[nodiscard]] Usuario obtenerPorDni(const std::string& dni) override;
  [[nodiscard]] Usuario autenticar(const std::string& dni, const std::string& password) override;
  [[nodiscard]] bool actualizar(const Usuario& usuario) override;
  [[nodiscard]] bool eliminar(const std::string& dni) override;
};

#endif  // USUARIOREPOSITORY_HPP