#include "db/database.hpp"
#include "db/db_exceptions.hpp"

#include <iostream>

SqliteDatabase::SqliteDatabase(const std::string& customPath) {
  if (!abrirSQL(customPath)) {
    throw db::DatabaseException(
        "Error al inicializar la base de datos SQLite en: " +
        (customPath.empty() ? "../data/cine.db" : customPath));
  }
}

SqliteDatabase::~SqliteDatabase() {
  cerrarSQL();
}

SqliteDatabase::SqliteDatabase(SqliteDatabase&& other) noexcept
    : db(other.db), dbPath(std::move(other.dbPath)) {
  other.db = nullptr;
}

SqliteDatabase& SqliteDatabase::operator=(SqliteDatabase&& other) noexcept {
  if (this != &other) {
    cerrarSQL();
    db = other.db;
    dbPath = std::move(other.dbPath);
    other.db = nullptr;
  }
  return *this;
}

bool SqliteDatabase::abrirSQL(const std::string& customPath) {
  std::lock_guard<std::mutex> lock(dbMutex);
  if (db != nullptr) {
    sqlite3_close_v2(db);
    db = nullptr;
  }

  if (!customPath.empty()) {
    dbPath = customPath;
  } else {
    dbPath = "../data/cine.db";
    try {
      std::filesystem::path exePath =
          std::filesystem::read_symlink("/proc/self/exe");
      std::filesystem::path dbTry =
          exePath.parent_path() / "../.." / "data" / "cine.db";
      if (std::filesystem::exists(dbTry)) {
        dbPath = std::filesystem::canonical(dbTry).string();
      }
    } catch (...) {
      // Continuar con rutas alternativas
    }
    if (!std::filesystem::exists(dbPath)) {
      const std::vector<std::string> potentialPaths = {
          "data/cine.db", "../data/cine.db", "../../data/cine.db"};
      for (const auto& path : potentialPaths) {
        if (std::filesystem::exists(path)) {
          dbPath = path;
          break;
        }
      }
    }
  }

  int rc = sqlite3_open(dbPath.c_str(), &db);

  if (rc != SQLITE_OK) {
    std::cerr << "No se pudo abrir la base de datos [" << dbPath << "]: "
              << (db ? sqlite3_errmsg(db) : "Error desconocido") << std::endl;
  } else {
    sqlite3_exec(db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);
    sqlite3_exec(db, "PRAGMA journal_mode = WAL;", nullptr, nullptr, nullptr);
    sqlite3_exec(db, "PRAGMA busy_timeout = 5000;", nullptr, nullptr, nullptr);
  }
  return rc == SQLITE_OK;
}

void SqliteDatabase::cerrarSQL() {
  std::lock_guard<std::mutex> lock(dbMutex);
  if (db != nullptr) {
    sqlite3_close_v2(db);
    db = nullptr;
  }
}

SqliteStatement::SqliteStatement(sqlite3* db, std::string_view query) {
  if (!db) {
    throw std::runtime_error("SqliteStatement: base de datos nula o no inicializada.");
  }
  int rc = sqlite3_prepare_v2(db, query.data(), static_cast<int>(query.size()), &stmt, nullptr);
  if (rc != SQLITE_OK) {
    throw std::runtime_error(std::string("Error al preparar la consulta: ") +
                             sqlite3_errmsg(db));
  }
}

SqliteStatement::~SqliteStatement() {
  if (stmt != nullptr) {
    sqlite3_finalize(stmt);
    stmt = nullptr;
  }
}

SqliteStatement::SqliteStatement(SqliteStatement&& other) noexcept
    : stmt(other.stmt) {
  other.stmt = nullptr;
}

SqliteStatement& SqliteStatement::operator=(SqliteStatement&& other) noexcept {
  if (this != &other) {
    if (stmt != nullptr) {
      sqlite3_finalize(stmt);
    }
    stmt = other.stmt;
    other.stmt = nullptr;
  }
  return *this;
}

bool SqliteStatement::bindInt(int index, int value) {
  return sqlite3_bind_int(stmt, index, value) == SQLITE_OK;
}

bool SqliteStatement::bindInt64(int index, sqlite3_int64 value) {
  return sqlite3_bind_int64(stmt, index, value) == SQLITE_OK;
}

bool SqliteStatement::bindFloat(int index, float value) {
  return sqlite3_bind_double(stmt, index, static_cast<double>(value)) == SQLITE_OK;
}

bool SqliteStatement::bindDouble(int index, double value) {
  return sqlite3_bind_double(stmt, index, value) == SQLITE_OK;
}

bool SqliteStatement::bindText(int index, std::string_view value) {
  return sqlite3_bind_text(stmt, index, value.data(), static_cast<int>(value.size()),
                           SQLITE_TRANSIENT) == SQLITE_OK;
}

bool SqliteStatement::bindNull(int index) {
  return sqlite3_bind_null(stmt, index) == SQLITE_OK;
}

int SqliteStatement::step() {
  return sqlite3_step(stmt);
}

void SqliteStatement::reset() {
  if (stmt != nullptr) {
    sqlite3_reset(stmt);
    sqlite3_clear_bindings(stmt);
  }
}

int SqliteStatement::getColumnInt(int index) const {
  return sqlite3_column_int(stmt, index);
}

sqlite3_int64 SqliteStatement::getColumnInt64(int index) const {
  return sqlite3_column_int64(stmt, index);
}

float SqliteStatement::getColumnFloat(int index) const {
  return static_cast<float>(sqlite3_column_double(stmt, index));
}

double SqliteStatement::getColumnDouble(int index) const {
  return sqlite3_column_double(stmt, index);
}

std::string SqliteStatement::getColumnText(int index) const {
  const auto* rawText = reinterpret_cast<const char*>(sqlite3_column_text(stmt, index));
  if (rawText != nullptr) {
    return std::string(rawText);
  }
  return "";
}

bool SqliteStatement::isColumnNull(int index) const {
  return sqlite3_column_type(stmt, index) == SQLITE_NULL;
}

SqliteTransaction::SqliteTransaction(sqlite3* database) : db(database) {
  if (db != nullptr) {
    int rc = sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr);
    if (rc != SQLITE_OK) {
      std::string err = sqlite3_errmsg(db);
      db = nullptr;
      throw db::DatabaseException("Error al iniciar transaccion SQLite: " + err);
    }
  }
}

SqliteTransaction::~SqliteTransaction() {
  if (db != nullptr && !committed) {
    sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
  }
}

SqliteTransaction::SqliteTransaction(SqliteTransaction&& other) noexcept
    : db(other.db), committed(other.committed) {
  other.db = nullptr;
  other.committed = true;
}

SqliteTransaction& SqliteTransaction::operator=(SqliteTransaction&& other) noexcept {
  if (this != &other) {
    if (db != nullptr && !committed) {
      sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
    }
    db = other.db;
    committed = other.committed;
    other.db = nullptr;
    other.committed = true;
  }
  return *this;
}

bool SqliteTransaction::commit() {
  if (db != nullptr && !committed) {
    int rc = sqlite3_exec(db, "COMMIT;", nullptr, nullptr, nullptr);
    if (rc == SQLITE_OK) {
      committed = true;
      return true;
    }
  }
  return false;
}

void SqliteTransaction::rollback() {
  if (db != nullptr && !committed) {
    sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
    committed = true;
  }
}