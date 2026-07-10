#include "db/database.hpp"

#include <iostream>

bool SqliteDatabase::abrirSQL() {
  std::string dbPath = "../data/cine.db";

  try {
    std::filesystem::path exePath =
        std::filesystem::read_symlink("/proc/self/exe");
    std::filesystem::path dbTry =
        exePath.parent_path() / "../.." / "data" / "cine.db";
    if (std::filesystem::exists(dbTry)) {
      dbPath = std::filesystem::canonical(dbTry).string();
    }
  } catch (...) {
    // Ignorar excepciones y continuar con otras opciones
  }
  if (!std::filesystem::exists(dbPath)) {
    std::vector<std::string> potentialPaths = {
        "data/cine.db", "../data/cine.db", "../../data/cine.db"};
    for (const auto& path : potentialPaths) {
      if (std::filesystem::exists(path)) {
        dbPath = path;
        break;
      }
    }
  }

  int rc = sqlite3_open(dbPath.c_str(), &db);

  if (rc != SQLITE_OK) {
    std::cerr << "No se pudo abrir la base de datos: " << sqlite3_errmsg(db)
              << std::endl;
  }
  return rc == SQLITE_OK;
}

void SqliteDatabase::cerrarSQL() {
  if (db != nullptr) {
    sqlite3_close(db);
    db = nullptr;
  }
}

SqliteStatement::SqliteStatement(sqlite3* db, const std::string& query) {
  int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
  if (rc != SQLITE_OK) {
    throw std::runtime_error(std::string("Error al preparar la consulta: ") +
                             sqlite3_errmsg(db));
  }
}

bool SqliteStatement::bindInt(int index, int value) {
  int rc = sqlite3_bind_int(stmt, index, value);
  if (rc != SQLITE_OK) {
    std::cerr << "Error al enlazar parámetro: "
              << sqlite3_errmsg(sqlite3_db_handle(stmt)) << std::endl;
    return false;
  }
  return true;
}

bool SqliteStatement::bindText(int index, const std::string& value) {
  int rc = sqlite3_bind_text(stmt, index, value.c_str(), -1, SQLITE_TRANSIENT);
  if (rc != SQLITE_OK) {
    std::cerr << "Error al enlazar parámetro: "
              << sqlite3_errmsg(sqlite3_db_handle(stmt)) << std::endl;
    return false;
  }
  return true;
}

int SqliteStatement::step() {
  int rc = sqlite3_step(stmt);
  if (rc != SQLITE_ROW && rc != SQLITE_DONE) {
    std::cerr << "Error en la ejecución de la consulta: "
              << sqlite3_errmsg(sqlite3_db_handle(stmt)) << std::endl;
  }
  return rc;
}

int SqliteStatement::getColumnInt(int index) {
  return sqlite3_column_int(stmt, index);
}
std::string SqliteStatement::getColumnText(int index) {
  const char* textoCrudo = (const char*)sqlite3_column_text(stmt, index);
  if (textoCrudo != nullptr) {
      return std::string(textoCrudo);
  } else {
      return ""; 
  }
}