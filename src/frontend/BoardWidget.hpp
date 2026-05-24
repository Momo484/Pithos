#pragma once
#include "Square.hpp"
#include <QWidget>

// Qwidget is the base class for anything drawn onto the screen in Qt.
// We inherit from it and override the methods we care about (OOP).
class BoardWidget : public QWidget {
  // Required for any class that uses QT features
  // Must be the very first line inside the class
  // a macro is a preprocessor directive that acts as a simple text replacement
  // tool
  Q_OBJECT;

public:
  // Standard Qt widget constructor - always takes an optional parent.
  // Parent owns this widget's memory, so we don'et need to delete it.
  // Explicit tells the compiler that the type must be compiled as stated
  explicit BoardWidget(QWidget *parent = nullptr);

  QSize sizeHint() const override;

protected:
  // Qt calls this automatically whenever the widget needs to be redrawn.
  // We use it to draw our chess board
  void paintEvent(QPaintEvent *event) override;

  // Qt calls this when the user clicks the mouse button down.
  void mousePressEvent(QMouseEvent *event) override;

  // When the mouse is moving.
  void mouseMoveEvent(QMouseEvent *event) override;

  // Qt calss this twhen the user released the moust button.
  void mouseReleaseEvent(QMouseEvent *event) override;

private:
  // Helps converts our pixel position on the screen to a board square.

  Square selectedSquare = {-1, -1};
  bool isDragging = false;
  QPoint dragPos;

  int squareSize() const;
  Square pixelToSquare(QPoint p) const;
  bool isValidSquare(Square square) const;
  void handleMove(Square from, Square to);
  void cancelSelection();
};
