#ifndef TIME_UTILS_HPP
#define TIME_UTILS_HPP

#include <ctime>

namespace utils {

// Convierte un time_t a std::tm de manera segura y reentrante para entornos multihilo
inline std::tm safeLocalTime(std::time_t timeVal) noexcept {
  std::tm result{};
#if defined(_WIN32) || defined(_WIN64)
  localtime_s(&result, &timeVal);
#else
  localtime_r(&timeVal, &result);
#endif
  return result;
}

}  // namespace utils

#endif  // TIME_UTILS_HPP
