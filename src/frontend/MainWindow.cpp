#include "MainWindow.hpp"
#include <QHBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
  setWindowTitle("Pithos");

  QWidget* central = new QWidget(this);
  setCentralWidget(central);

  QHBoxLayout* layout = new QHBoxLayout(central);
  layout->setSpacing(0);
  layout->setContentsMargins(0,0,0,0);
  
  board = new BoardWidget(game, this);
  panel = new SidePanel(game, this);

  layout->addWidget(board);
  layout->addWidget(panel);
  
  // connecting signals and slots stuff
  connect(panel, &SidePanel::gameStateChanged, board, &BoardWidget::refresh);
  connect(panel, &SidePanel::gameStateChanged, panel, &SidePanel::refresh);
  connect(board, &BoardWidget::gameStateChanged, panel, &SidePanel::refresh);

  setFixedSize(sizeHint());
}
