#include <QApplication>
#include <QCoreApplication>

#include "app/AppLogger.hpp"
#include "app/MainWindow.hpp"

int main(int argc, char* argv[]) {
  QCoreApplication::setApplicationName("Configured");
  QCoreApplication::setOrganizationName("Configured");

  QApplication app(argc, argv);

  QString loggingError;
  if (!AppLogger::install(&loggingError)) {
    qWarning() << loggingError;
  }

  MainWindow window;
  window.show();

  return app.exec();
}
