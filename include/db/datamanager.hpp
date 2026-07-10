#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <sqlite3.h>

#include <filesystem>
#include <string>
#include <vector>

class SqliteDatabase {
 private:
  sqlite3* db;

 public:
  SqliteDatabase() { abrirSQL(); };
  ~SqliteDatabase() { cerrarSQL(); };

  bool abrirSQL();
  void cerrarSQL();

  sqlite3* getDb() const { return db; }
};

class SqliteStatement {
 private:
  sqlite3_stmt* stmt;

 public:
  SqliteStatement(sqlite3* db, const std::string& query);
  ~SqliteStatement() { sqlite3_finalize(stmt); }

  bool bindInt(int index, int value);
  bool bindText(int index, const std::string& value);

  int step();

  int getColumnInt(int index);
  std::string getColumnText(int index);
};

#endif  // DATABASE_HPP