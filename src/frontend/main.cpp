#include <QApplication>
#include "MainWindow.hpp"

int main(int argc, char *argv[]) {
  // Qapplication must be created before any widgets
  // It takes argc/argv, because Qt accpets some command line arguments
  QApplication app(argc, argv);

  MainWindow window;
  window.show();

  // app.exec starts the event loop.
  // It blocks here until the window is closed, then returns.
  return app.exec();
}

