#include <QApplication>
#include <QFile>
#include <QMainWindow>

#include "mainwindow.h"

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);

  std::vector<QString> potentialPaths = {
      "apps/gui/ui/style.qss",
      "../apps/gui/ui/style.qss",
      "../../apps/gui/ui/style.qss",
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
  window.resize(800, 600);
  window.show();

  return app.exec();
}
