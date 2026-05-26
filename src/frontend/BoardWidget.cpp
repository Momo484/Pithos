#include "BoardWidget.hpp"
#include "Square.hpp"
#include <QDebug>      // gives us qDebug() for printing, like cout
#include <QMouseEvent> // gives us access to mouse click positions
#include <QPainter>    // We use this to draw things
#include <string>
#include <unordered_set>

// --- Constructor

BoardWidget::BoardWidget(Game& game, QWidget *parent) : game(game), QWidget(parent) {
  // Enables mouse tracking so we get mouseMoveEvents without a button holds
  setMouseTracking(true);
  setFocusPolicy(Qt::StrongFocus);
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

QSize BoardWidget::sizeHint() const { return QSize(800, 800); }

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

    // precompute hints once — note the rank flip on selectedSquare
    Square backendSelected = {selectedSquare.x, 7 - selectedSquare.y};
    std::unordered_set<Square, SquareHash> moveHints = 
        game.getLegalDestinations(backendSelected);

    // --- pass 1: draw squares ---
    for (int r = 0; r < 8; r++) {
        for (int f = 0; f < 8; f++) {
            Square sq = {f, r};
            bool isLight = (r + f) % 2 == 0;
            QColor color = isLight ? QColor(240, 217, 181) : QColor(181, 136, 99);
            if (sq == selectedSquare) {
                color = QColor(72, 92, 130);
            }
            painter.fillRect(f * sz, r * sz, sz, sz, color);
        }
    }

    // --- pass 2: draw pieces ---
    for (int r = 0; r < 8; r++) {
        for (int f = 0; f < 8; f++) {
            Square sq = {f, r};
            // skip the dragged piece's origin square
            if (isDragging && sq == selectedSquare) continue;
            char tile = game.getSquare(f, 7 - r);
            if (tile != ' ' && piecePixmaps.count(tile)) {
                painter.drawPixmap(f * sz, r * sz, sz, sz, piecePixmaps.at(tile));
            }
        }
    }

    // --- pass 3: draw move hints on top of everything ---
    painter.setPen(Qt::NoPen);
    for (int r = 0; r < 8; r++) {
        for (int f = 0; f < 8; f++) {
            // hints are in backend coordinates, so flip r for lookup
            Square backendSq = {f, 7 - r};
            if (!moveHints.count(backendSq)) continue;

            char tile = game.getSquare(f, 7 - r);
            if (tile == ' ') {
                // empty square — draw a small centred dot
                painter.setBrush(QColor(0, 0, 0, 80));
                int dotSize = sz / 3;
                int offset  = (sz - dotSize) / 2;
                painter.drawEllipse(f * sz + offset, r * sz + offset, 
                                    dotSize, dotSize);
            } else {
                // occupied square — draw a ring around the edge
                painter.setBrush(Qt::NoBrush);
                painter.setPen(QPen(QColor(0, 0, 0, 80), 4));
                int margin = 4;
                painter.drawEllipse(f * sz + margin, r * sz + margin,
                                    sz - margin * 2, sz - margin * 2);
                painter.setPen(Qt::NoPen);
            }
        }
    }

    // --- pass 4: draw ghost piece ---
    if (isDragging && isValidSquare(selectedSquare)) {
        char dragged = game.getSquare(selectedSquare.x, 7 - selectedSquare.y);
        if (dragged != ' ' && piecePixmaps.count(dragged)) {
            painter.drawPixmap(dragPos.x() - sz / 2, dragPos.y() - sz / 2,
                               sz, sz, piecePixmaps.at(dragged));
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
  GameResult result = game.getResult();
  if (result == GameResult::Draw) {
    qDebug() << "Drawn Game!" << Qt::endl;
  } else if (result == GameResult::WhiteWins) {
    qDebug() << "White Wins!" << Qt::endl;
  } else if (result == GameResult::BlackWins) {
    qDebug() << "Black Wins!" << Qt::endl;
  }
  refresh();
  emit gameStateChanged();
}

// --- CancelSelection()

void BoardWidget::cancelSelection() {
  selectedSquare = {-1, -1};
  isDragging = false;
}

// --- Refresh

void BoardWidget::refresh() {
  cancelSelection();
  update();
}
