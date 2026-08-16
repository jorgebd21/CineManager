#ifndef SECURITY_HPP
#define SECURITY_HPP

#include <string>
#include <string_view>

namespace security {

// Genera una sal criptográfica aleatoria en formato hexadecimal
[[nodiscard]] std::string generateSalt(std::size_t bytes = 16);

// Calcula el hash SHA-256 de una cadena de texto
[[nodiscard]] std::string sha256(std::string_view input);

// Hashea una contraseña combinándola con una sal criptográfica (formato: "sal$hash")
[[nodiscard]] std::string hashPassword(std::string_view password, std::string_view salt = "");

// Verifica si la contraseña en texto plano coincide con el hash almacenado
[[nodiscard]] bool verifyPassword(std::string_view rawPassword, std::string_view storedHash);

// Comparación en tiempo constante para mitigar ataques de temporización (timing attacks)
[[nodiscard]] bool constantTimeEquals(std::string_view a, std::string_view b) noexcept;

}  // namespace security

#endif  // SECURITY_HPP
