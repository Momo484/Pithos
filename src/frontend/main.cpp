#include <QApplication>
#include "BoardWidget.hpp"

int main(int argc, char *argv[]) {
  // Qapplication must be created before any widgets
  // It takes argc/argv, because Qt accpets some command line arguments
  QApplication app(argc, argv);

  // create our baord widget with no parent - this makes it a top-level window.
  BoardWidget window;
  window.setWindowTitle("Chess");
  window.adjustSize();
  window.setFixedSize(window.sizeHint());
  window.show(); // hidden by default

  // app.exec starts the event loop.
  // It blocks here until the window is closed, then returns.
  return app.exec();
}

