#include "utils/security.hpp"

#include <array>
#include <cstdint>
#include <iomanip>
#include <random>
#include <sstream>
#include <string>
#include <vector>

namespace security {

namespace {

// Funciones auxiliares estándar SHA-256 (FIPS 180-4)
inline uint32_t rotr(uint32_t x, uint32_t n) noexcept {
  return (x >> n) | (x << (32 - n));
}
inline uint32_t choose(uint32_t e, uint32_t f, uint32_t g) noexcept {
  return (e & f) ^ (~e & g);
}
inline uint32_t majority(uint32_t a, uint32_t b, uint32_t c) noexcept {
  return (a & b) ^ (a & c) ^ (b & c);
}
inline uint32_t sig0(uint32_t x) noexcept {
  return rotr(x, 2) ^ rotr(x, 13) ^ rotr(x, 22);
}
inline uint32_t sig1(uint32_t x) noexcept {
  return rotr(x, 6) ^ rotr(x, 11) ^ rotr(x, 25);
}
inline uint32_t theta0(uint32_t x) noexcept {
  return rotr(x, 7) ^ rotr(x, 18) ^ (x >> 3);
}
inline uint32_t theta1(uint32_t x) noexcept {
  return rotr(x, 17) ^ rotr(x, 19) ^ (x >> 10);
}

constexpr std::array<uint32_t, 64> K = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1,
    0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786,
    0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,
    0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b,
    0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a,
    0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

}  // namespace

std::string sha256(std::string_view input) {
  std::vector<uint8_t> data(input.begin(), input.end());
  uint64_t bitLen = data.size() * 8;

  // Padding
  data.push_back(0x80);
  while ((data.size() % 64) != 56) {
    data.push_back(0x00);
  }

  for (int i = 7; i >= 0; --i) {
    data.push_back(static_cast<uint8_t>((bitLen >> (i * 8)) & 0xFF));
  }

  uint32_t h0 = 0x6a09e667;
  uint32_t h1 = 0xbb67ae85;
  uint32_t h2 = 0x3c6ef372;
  uint32_t h3 = 0xa54ff53a;
  uint32_t h4 = 0x510e527f;
  uint32_t h5 = 0x9b05688c;
  uint32_t h6 = 0x1f83d9ab;
  uint32_t h7 = 0x5be0cd19;

  for (std::size_t chunk = 0; chunk < data.size(); chunk += 64) {
    std::array<uint32_t, 64> w{};
    for (std::size_t i = 0; i < 16; ++i) {
      w[i] = (static_cast<uint32_t>(data[chunk + i * 4]) << 24) |
             (static_cast<uint32_t>(data[chunk + i * 4 + 1]) << 16) |
             (static_cast<uint32_t>(data[chunk + i * 4 + 2]) << 8) |
             (static_cast<uint32_t>(data[chunk + i * 4 + 3]));
    }
    for (std::size_t i = 16; i < 64; ++i) {
      w[i] = theta1(w[i - 2]) + w[i - 7] + theta0(w[i - 15]) + w[i - 16];
    }

    uint32_t a = h0;
    uint32_t b = h1;
    uint32_t c = h2;
    uint32_t d = h3;
    uint32_t e = h4;
    uint32_t f = h5;
    uint32_t g = h6;
    uint32_t h = h7;

    for (std::size_t i = 0; i < 64; ++i) {
      uint32_t t1 = h + sig1(e) + choose(e, f, g) + K[i] + w[i];
      uint32_t t2 = sig0(a) + majority(a, b, c);
      h = g;
      g = f;
      f = e;
      e = d + t1;
      d = c;
      c = b;
      b = a;
      a = t1 + t2;
    }

    h0 += a;
    h1 += b;
    h2 += c;
    h3 += d;
    h4 += e;
    h5 += f;
    h6 += g;
    h7 += h;
  }

  std::ostringstream oss;
  oss << std::hex << std::setfill('0');
  for (uint32_t val : {h0, h1, h2, h3, h4, h5, h6, h7}) {
    oss << std::setw(8) << val;
  }
  return oss.str();
}

std::string generateSalt(std::size_t bytes) {
  std::random_device rd;
  std::mt19937_64 gen(rd());
  std::uniform_int_distribution<uint32_t> dist(0, 255);

  std::ostringstream oss;
  oss << std::hex << std::setfill('0');
  for (std::size_t i = 0; i < bytes; ++i) {
    oss << std::setw(2) << dist(gen);
  }
  return oss.str();
}

std::string hashPassword(std::string_view password, std::string_view salt) {
  std::string actualSalt = salt.empty() ? generateSalt(16) : std::string(salt);
  std::string combined = actualSalt + ":" + std::string(password);
  std::string digest = sha256(combined);
  return actualSalt + "$" + digest;
}

bool constantTimeEquals(std::string_view a, std::string_view b) noexcept {
  if (a.size() != b.size()) {
    return false;
  }
  unsigned char result = 0;
  for (std::size_t i = 0; i < a.size(); ++i) {
    result |= static_cast<unsigned char>(a[i] ^ b[i]);
  }
  return result == 0;
}

bool verifyPassword(std::string_view rawPassword, std::string_view storedHash) {
  if (storedHash.empty() || rawPassword.empty()) {
    return false;
  }

  // Comprobar si el formato es "sal$hash"
  auto delimPos = storedHash.find('$');
  if (delimPos != std::string_view::npos) {
    std::string_view salt = storedHash.substr(0, delimPos);
    std::string expectedHash = hashPassword(rawPassword, salt);
    return constantTimeEquals(expectedHash, storedHash);
  }

  // Compatibilidad con contraseñas legadas en texto plano
  return constantTimeEquals(rawPassword, storedHash);
}

}  // namespace security
