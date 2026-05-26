#pragma once
#include "BoardWidget.hpp"
#include "Game.hpp"
#include "SidePanel.hpp"
#include <QMainWindow>

class MainWindow : public QMainWindow {
  Q_OBJECT
public:
  explicit MainWindow(QWidget *parent = nullptr);

private:
  Game game;
  BoardWidget *board;
  SidePanel *panel;
};
