#pragma once
#include "Game.hpp"
#include <QLabel>
#include <QWidget>

class SidePanel : public QWidget {
  Q_OBJECT
public:
  explicit SidePanel(Game &game, QWidget *parent = nullptr);

  // Called after game state change to update the status label.
  void refresh();

signals:
  void gameStateChanged();

private slots:
  void onResetClicked();
  void onUndoClicked();

private:
  Game &game;
  QLabel *statusLabel;
};
