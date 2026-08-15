#ifndef QRHELPER_H
#define QRHELPER_H

#include <QColor>
#include <QImage>
#include <QPixmap>
#include <QString>
#include "utils/qrcodegen.hpp"

class QrHelper {
 public:
  static QPixmap generarQR(const QString& texto, int tamanoDeseado = 180);
};

#endif  // QRHELPER_H
