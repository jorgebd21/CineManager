#include <QApplication>
#include <QFile>
#include <QMainWindow>

#include "mainwindow.h"

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);

  QFile styleFile("apps/gui/ui/style.qss");
  if (styleFile.open(QFile::ReadOnly)) {
    QString styleSheet = QLatin1String(styleFile.readAll());
    app.setStyleSheet(styleSheet);
    styleFile.close();
  }

  MainWindow window;
  window.setWindowTitle("CineManager");
  window.resize(800, 600);
  window.show();

  return app.exec();
}
