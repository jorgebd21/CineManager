#include <QApplication>
#include <QFile>
#include <QMainWindow>
#include <QPalette>
#include <QString>
#include <vector>

#include "mainwindow.h"

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);

  // Paleta oscura base para garantizar consistencia visual en controles nativos y modales
  QPalette darkPalette;
  darkPalette.setColor(QPalette::Window, QColor(18, 20, 29));
  darkPalette.setColor(QPalette::WindowText, Qt::white);
  darkPalette.setColor(QPalette::Base, QColor(26, 29, 40));
  darkPalette.setColor(QPalette::AlternateBase, QColor(18, 20, 29));
  darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
  darkPalette.setColor(QPalette::ToolTipText, Qt::white);
  darkPalette.setColor(QPalette::Text, Qt::white);
  darkPalette.setColor(QPalette::PlaceholderText, QColor(92, 99, 112));
  darkPalette.setColor(QPalette::Button, QColor(30, 32, 45));
  darkPalette.setColor(QPalette::ButtonText, Qt::white);
  darkPalette.setColor(QPalette::BrightText, Qt::red);
  darkPalette.setColor(QPalette::Link, QColor(97, 175, 239));
  darkPalette.setColor(QPalette::Highlight, QColor(229, 192, 123));
  darkPalette.setColor(QPalette::HighlightedText, QColor(18, 20, 29));
  app.setPalette(darkPalette);

  std::vector<QString> potentialPaths = {
      "apps/gui/ui/style.qss",
      "../apps/gui/ui/style.qss",
      "../../apps/gui/ui/style.qss",
      QApplication::applicationDirPath() + "/../apps/gui/ui/style.qss",
      QApplication::applicationDirPath() + "/../../apps/gui/ui/style.qss",
      "/home/jorgebd/Documentos/Proyectos/CineManager/apps/gui/ui/style.qss",
      "/workspaces/CineManager/apps/gui/ui/style.qss"
  };
  
  for (const auto& path : potentialPaths) {
    QFile styleFile(path);
    if (styleFile.open(QFile::ReadOnly)) {
      QString styleSheet = QLatin1String(styleFile.readAll());
      app.setStyleSheet(styleSheet);
      styleFile.close();
      break;
    }
  }

  MainWindow window;
  window.setWindowTitle("CineManager");
  window.resize(1024, 768);
  window.show();

  return app.exec();
}
