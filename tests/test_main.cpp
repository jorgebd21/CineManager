#include <gtest/gtest.h>
#include <filesystem>

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  int result = RUN_ALL_TESTS();
  std::error_code ec;
  std::filesystem::remove("test_cine.db", ec);
  std::filesystem::remove("test_cine.db-wal", ec);
  std::filesystem::remove("test_cine.db-shm", ec);
  return result;
}
