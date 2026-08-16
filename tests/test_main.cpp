#include <gtest/gtest.h>
#include <filesystem>
#include <vector>

void cleanupTestFiles() {
  const std::vector<std::string> testDbs = {
      "test_cine.db", "test_cine.db-wal", "test_cine.db-shm",
      "test_integrity.db", "test_integrity.db-wal", "test_integrity.db-shm",
      "test_toctou.db", "test_toctou.db-wal", "test_toctou.db-shm",
      "test_domain.db", "test_domain.db-wal", "test_domain.db-shm"
  };
  std::error_code ec;
  for (const auto& file : testDbs) {
    std::filesystem::remove(file, ec);
  }
}

int main(int argc, char** argv) {
  cleanupTestFiles();
  ::testing::InitGoogleTest(&argc, argv);
  int result = RUN_ALL_TESTS();
  cleanupTestFiles();
  return result;
}
