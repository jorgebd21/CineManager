# 🖼️ Guía e Índice de Assets Visuales — CineManager

> Directorio de recursos multimedia utilizados en el [`README.md` principal](../../README.md) y en [`docs/dev_documentation.md`](../dev_documentation.md).

---

## 📁 Archivos Disponibles

Capturas de pantalla reales tomadas directamente desde **CineManagerGUI** (Qt6, tema oscuro `style.qss`):

| Archivo | Pantalla que representa |
| :--- | :--- |
| `login.jpg` | 🔐 Modal de inicio de sesión por DNI y contraseña |
| `register.jpg` | 📝 Formulario de registro de nueva cuenta de usuario |
| `cine.jpg` | 🏛️ Pantalla de selección de complejo de cine (`CineCardWidget`) |
| `cartelera.jpg` | 🎬 Cartelera de películas con búsqueda y filtrado por género (`MovieCardWidget`) |
| `sesion.jpg` | 📅 Selección de sesión: horarios agrupados por día con sala asignada |
| `sala.jpg` | 💺 Mapa interactivo de butacas (Libre 🟢 / Ocupada 🔴 / Tu Selección 🟡) |
| `tipo_butaca.jpg` | 🎟️ Diálogo `TarifasDialog`: desglose de tarifas dinámicas por butaca |
| `ticket.jpg` | 🎫 Ticket de compra con código QR real escaneable (motor Nayuki ISO/IEC 18004) |
| `error_api.jng` | ⚠️ Diálogo de error de conexión cuando el servidor REST API no está activo |

> 🤖 **Nota:** Las imágenes de portada de los complejos de cine y los carteles de las películas mostrados en la aplicación han sido **generados con Inteligencia Artificial** con fines demostrativos.

---

## 📐 Directrices de Contribución

1. **Formato preferido:** `.jpg` o `.webp` para capturas de pantalla; `.png` para diagramas con transparencia.
2. **Resolución:** Mínimo 1024 px de ancho. Preferiblemente capturas a resolución nativa de pantalla.
3. **Tamaño máximo:** Mantener cada archivo por debajo de **2 MB**. Usar `jpegoptim`, `pngquant` o `cwebp` para comprimir antes de subir.
4. **Tema visual:** Todas las capturas deben tomarse con el tema oscuro por defecto de la aplicación (`apps/gui/ui/style.qss`).
5. **Nomenclatura:** Usar nombres descriptivos en minúsculas con guiones bajos (ej. `mapa_sala_llena.jpg`).
