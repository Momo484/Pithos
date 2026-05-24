#include "BoardWidget.hpp"
#include "Square.hpp"
#include <QDebug>      // gives us qDebug() for printing, like cout
#include <QMouseEvent> // gives us access to mouse click positions
#include <QPainter>    // We use this to draw things
#include <string>

// --- Constructor

BoardWidget::BoardWidget(QWidget *parent) : QWidget(parent) {
  // Enables mouse tracking so we get mouseMoveEvents without a button holds
  setMouseTracking(true);
  for (char c : {'P', 'N', 'B', 'R', 'Q', 'K', 'p', 'n', 'b', 'r', 'q', 'k'}) {
    std::string filename = ":/pieces/";
    filename += (char)std::tolower(c); // piece letter — p, n, b, r, q, k
    filename +=
        std::isupper(c) ? 'w' : 'b'; // colour — w for white, b for black
    filename += ".svg";
    piecePixmaps[c] = QPixmap(QString::fromStdString(filename));
  }
  for (auto &[ch, px] : piecePixmaps) {
    if (px.isNull())
      qDebug() << "Failed to load piece:" << ch;
  }
  game.reset();
}

// --- Helper

int BoardWidget::squareSize() const {
  // we derive square size from the widget width so it is always correct.
  // width() is a QWidget method that returns the current pixel width.
  return width() / 8;
}

// --- sizeHint

QSize BoardWidget::sizeHint() const { return QSize(480, 480); }

// --- PixeltoSquare

Square BoardWidget::pixelToSquare(QPoint p) const {
  int sz = squareSize();
  return {p.x() / sz, p.y() / sz};
}

// --- isValid Square

bool BoardWidget::isValidSquare(Square sq) const {
  return sq.x >= 0 && sq.x < 8 && sq.y >= 0 && sq.y < 8;
}

// --- paintEvent
// Called every time the board needs to be redrawn
// call update() to request a redraw, never call ourselves.

void BoardWidget::paintEvent(QPaintEvent *) {
  QPainter painter(this);
  int sz = squareSize();

  for (int r = 0; r < 8; r++) {
    for (int f = 0; f < 8; f++) {
      Square sq = {f, r};
      bool isLight = (r + f) % 2 == 0;
      QColor color = isLight ? QColor(240, 217, 181) : QColor(181, 136, 99);
      if (sq == selectedSquare) {
        color = QColor(100, 200, 100);
      }
      painter.fillRect(f * sz, r * sz, sz, sz, color);
      char tile = game.getSquare(f, 7 - r);
      if (!isDragging || sq != selectedSquare) {
        painter.drawPixmap(f * sz, r * sz, sz, sz, piecePixmaps[tile]);
      }
    }
  }

  // drawing a ghost piece
  if (isDragging && isValidSquare(selectedSquare)) {
    char dragged = game.getSquare(selectedSquare.x, 7 - selectedSquare.y);
    if (dragged != ' ' && piecePixmaps.count(dragged)) {
      painter.drawPixmap(dragPos.x() - sz / 2, dragPos.y() - sz / 2, sz, sz,
                         piecePixmaps[dragged]);
    }
  }
}

// --- mousePressEvent
// Called when the user presses the mouse button down

void BoardWidget::mousePressEvent(QMouseEvent *event) {
  if (event->button() != Qt::LeftButton)
    return;
  Square clicked = pixelToSquare(event->pos());
  if (!isValidSquare(clicked))
    return;

  if (selectedSquare.x == -1) {
    // nothing selected yet, so we select this square
    selectedSquare = clicked;
    isDragging = false;
    dragPos = event->pos();
  } else {
    // we had something selected
    // so we treat this as a destination click
    if (selectedSquare != clicked) {
      handleMove(selectedSquare, clicked);
    } else {
      cancelSelection();
    }
  }

  // requesting a repaint
  update();
}

// --- mouseMoveEvent
// Called whenever them mouse moves (with button held, since we are not using
// full tracking for drags yet)

void BoardWidget::mouseMoveEvent(QMouseEvent *event) {
  if (!(event->buttons() & Qt::LeftButton))
    return;
  if (selectedSquare.x == -1)
    return;

  if (!isDragging && (event->pos() - dragPos).manhattanLength() > 6) {
    isDragging = true;
  }

  if (isDragging) {
    dragPos = event->pos();
    update();
  }
}

// --- mouseReleaseEvent
// Called when the user lets go of the mouse button

void BoardWidget::mouseReleaseEvent(QMouseEvent *event) {
  if (event->button() != Qt::LeftButton)
    return;
  if (selectedSquare.x == -1)
    return;
  Square released = pixelToSquare(event->pos());
  if (isDragging) {
    // drag complete - destination is where they let go.
    if (isValidSquare(released) && !(released == selectedSquare)) {
      handleMove(selectedSquare, released);
    } else {
      // dropped on the same square or off board
      cancelSelection();
    }
    isDragging = false;
  }
  // if we were not dragging, we are in click mode
  // the second click is handled in mousePress event not here
  // so we do nothing
  update();
}

// --- handleMove

void BoardWidget::handleMove(Square from, Square to) {
  std::string uci;
  uci += static_cast<char>('a' + from.x);
  uci += static_cast<char>('1' + (7 - from.y));

  uci += static_cast<char>('a' + to.x);
  uci += static_cast<char>('1' + (7 - to.y));

  qDebug() << "UCI:" << QString::fromStdString(uci);
  game.submitMove(uci);
  cancelSelection();
}

// --- CancelSelection()

void BoardWidget::cancelSelection() {
  selectedSquare = {-1, -1};
  isDragging = false;
}
