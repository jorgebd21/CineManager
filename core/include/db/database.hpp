#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <sqlite3.h>

#include <filesystem>
#include <memory>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

class SqliteDatabase {
 private:
  sqlite3* db{nullptr};
  std::string dbPath;
  mutable std::mutex dbMutex;

 public:
  explicit SqliteDatabase(const std::string& customPath = "");
  ~SqliteDatabase();

  // Rule of 5: Non-copyable, movable
  SqliteDatabase(const SqliteDatabase&) = delete;
  SqliteDatabase& operator=(const SqliteDatabase&) = delete;
  SqliteDatabase(SqliteDatabase&& other) noexcept;
  SqliteDatabase& operator=(SqliteDatabase&& other) noexcept;

  bool abrirSQL(const std::string& customPath = "");
  void cerrarSQL();

  [[nodiscard]] bool estaAbierta() const noexcept { return db != nullptr; }
  [[nodiscard]] sqlite3* getDb() const noexcept { return db; }
  [[nodiscard]] const std::string& getDbPath() const noexcept { return dbPath; }
  [[nodiscard]] std::mutex& getMutex() const noexcept { return dbMutex; }
};

class SqliteStatement {
 private:
  sqlite3_stmt* stmt{nullptr};

 public:
  SqliteStatement(sqlite3* db, std::string_view query);
  ~SqliteStatement();

  // Rule of 5: Non-copyable, movable
  SqliteStatement(const SqliteStatement&) = delete;
  SqliteStatement& operator=(const SqliteStatement&) = delete;
  SqliteStatement(SqliteStatement&& other) noexcept;
  SqliteStatement& operator=(SqliteStatement&& other) noexcept;

  bool bindInt(int index, int value);
  bool bindInt64(int index, sqlite3_int64 value);
  bool bindFloat(int index, float value);
  bool bindDouble(int index, double value);
  bool bindText(int index, std::string_view value);
  bool bindNull(int index);

  template <typename T>
  bool bind(int index, T&& val) {
    using Decayed = std::decay_t<T>;
    if constexpr (std::is_same_v<Decayed, int>) {
      return bindInt(index, val);
    } else if constexpr (std::is_same_v<Decayed, long> || std::is_same_v<Decayed, long long>) {
      return bindInt64(index, static_cast<sqlite3_int64>(val));
    } else if constexpr (std::is_same_v<Decayed, float>) {
      return bindFloat(index, val);
    } else if constexpr (std::is_same_v<Decayed, double>) {
      return bindDouble(index, val);
    } else if constexpr (std::is_constructible_v<std::string_view, Decayed>) {
      return bindText(index, std::string_view(val));
    } else if constexpr (std::is_same_v<Decayed, std::nullptr_t>) {
      return bindNull(index);
    } else {
      static_assert(!sizeof(T), "Unsupported bind type");
    }
  }

  int step();
  void reset();

  [[nodiscard]] int getColumnInt(int index) const;
  [[nodiscard]] sqlite3_int64 getColumnInt64(int index) const;
  [[nodiscard]] float getColumnFloat(int index) const;
  [[nodiscard]] double getColumnDouble(int index) const;
  [[nodiscard]] std::string getColumnText(int index) const;
  [[nodiscard]] bool isColumnNull(int index) const;

  [[nodiscard]] sqlite3_stmt* getHandle() const noexcept { return stmt; }
};

class SqliteTransaction {
 private:
  sqlite3* db{nullptr};
  bool committed{false};

 public:
  explicit SqliteTransaction(sqlite3* database);
  ~SqliteTransaction();

  SqliteTransaction(const SqliteTransaction&) = delete;
  SqliteTransaction& operator=(const SqliteTransaction&) = delete;
  SqliteTransaction(SqliteTransaction&& other) noexcept;
  SqliteTransaction& operator=(SqliteTransaction&& other) noexcept;

  bool commit();
  void rollback();
};

#endif  // DATABASE_HPP