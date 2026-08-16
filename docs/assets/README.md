# Assets Visuales — CineManager Documentation

Este directorio almacena los recursos multimedia (capturas de pantalla, diagramas y demostraciones animadas) utilizados en el `README.md` principal y la documentación técnica.

## Estructura de Directorios

- `screenshots/`: Capturas de pantalla en alta resolución (1920x1080 o escaladas a 2x) en formato WebP o PNG optimizado.
  - `01_seleccion_cine_cartelera.png`: Vista principal de selección de multicines y cartelera.
  - `02_mapa_sala_butacas.png`: Mapa interactivo de la sala con selección múltiple de butacas.
  - `03_modal_tarifas_ticket_qr.png`: Diálogo modal de tarifas dinámicas y ticket final con código QR.
  - `04_login_autenticacion_dni.png`: Modal de autenticación de usuario y pasarela de checkout.
- `gifs/`: Demostraciones animadas del flujo de usuario (grabaciones a 60fps optimizadas).
  - `booking_flow_demo.gif`: Flujo completo desde la selección de butacas hasta la emisión del ticket.
  - `auth_gatekeeper_demo.gif`: Flujo de validación de identidad en taquilla.
- `diagrams/`: Exportaciones vectoriales (SVG) o imágenes de alta definición de diagramas arquitectónicos y modelos entidad-relación.

## Directrices de Formato
1. **Compresión**: Utilizar herramientas de optimización (`pngquant`, `cwebp`, `gifsicle`) para mantener los archivos por debajo de 2MB.
2. **Tema Visual**: Las capturas deben tomarse utilizando el tema oscuro por defecto (`style.qss`).
