#include "qrhelper.h"

QPixmap QrHelper::generarQR(const QString& texto, int tamanoDeseado) {
  using namespace qrcodegen;
  try {
    QrCode qr = QrCode::encodeText(texto.toUtf8().constData(), QrCode::Ecc::MEDIUM);
    int qrSize = qr.getSize();
    int margin = 4; // Quiet zone estándar ISO/IEC 18004
    int totalSize = qrSize + margin * 2;

    QImage image(totalSize, totalSize, QImage::Format_RGB32);
    image.fill(Qt::white);

    for (int y = 0; y < qrSize; y++) {
      for (int x = 0; x < qrSize; x++) {
        if (qr.getModule(x, y)) {
          image.setPixelColor(x + margin, y + margin, QColor(15, 17, 23));
        } else {
          image.setPixelColor(x + margin, y + margin, Qt::white);
        }
      }
    }

    // Escalar con Nearest Neighbor (FastTransformation) para no desdibujar ni difuminar los módulos del QR
    return QPixmap::fromImage(image.scaled(
        tamanoDeseado, tamanoDeseado, Qt::KeepAspectRatio, Qt::FastTransformation));
  } catch (...) {
    return QPixmap();
  }
}
