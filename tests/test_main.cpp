#include <gtest/gtest.h>

// Punto de entrada opcional si se usa gtest_main, pero tenerlo explicitado permite configuraciones futuras
int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
