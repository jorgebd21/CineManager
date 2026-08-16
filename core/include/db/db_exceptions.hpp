#ifndef DB_EXCEPTIONS_HPP
#define DB_EXCEPTIONS_HPP

#include <stdexcept>
#include <string>

namespace db {

class DatabaseException : public std::runtime_error {
 public:
  explicit DatabaseException(const std::string& message)
      : std::runtime_error(message) {}
};

class ConstraintViolationException : public DatabaseException {
 public:
  explicit ConstraintViolationException(const std::string& message)
      : DatabaseException(message) {}
};

class NotFoundException : public DatabaseException {
 public:
  explicit NotFoundException(const std::string& message)
      : DatabaseException(message) {}
};

}  // namespace db

#endif  // DB_EXCEPTIONS_HPP
