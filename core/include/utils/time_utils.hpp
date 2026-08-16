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

// Formatea fechas garantizando idioma español sin depender de la configuración regional del SO
inline std::string formatearFechaEspanol(std::time_t timeVal, bool incluirHora = false) {
  std::tm tm = safeLocalTime(timeVal);
  static constexpr const char* diasSemana[] = {
      "Domingo", "Lunes", "Martes", "Miércoles", "Jueves", "Viernes", "Sábado"
  };
  static constexpr const char* meses[] = {
      "Enero", "Febrero", "Marzo", "Abril", "Mayo", "Junio",
      "Julio", "Agosto", "Septiembre", "Octubre", "Noviembre", "Diciembre"
  };

  int diaSemanaIdx = (tm.tm_wday >= 0 && tm.tm_wday <= 6) ? tm.tm_wday : 0;
  int mesIdx = (tm.tm_mon >= 0 && tm.tm_mon <= 11) ? tm.tm_mon : 0;

  char timeBuf[16];
  std::snprintf(timeBuf, sizeof(timeBuf), "%02d:%02d", tm.tm_hour, tm.tm_min);

  std::string resultado = std::string(diasSemana[diaSemanaIdx]) + ", " +
                          std::to_string(tm.tm_mday) + " de " +
                          meses[mesIdx];
  if (incluirHora) {
    resultado += " - " + std::string(timeBuf);
  }
  return resultado;
}

}  // namespace utils

#endif  // TIME_UTILS_HPP
