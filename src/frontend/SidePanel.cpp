#include "SidePanel.hpp"
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

SidePanel::SidePanel(Game &game, QWidget *parent)
    : game(game), QWidget(parent) {
  setFixedWidth(180);

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setAlignment(Qt::AlignTop);
  layout->setContentsMargins(16, 24, 16, 24);
  layout->setSpacing(10);

  // Title
  QLabel *title = new QLabel("Pithos");
  title->setAlignment(Qt::AlignCenter);
  QFont titleFont = title->font();
  titleFont.setPointSize(18);
  titleFont.setBold(true);
  title->setFont(titleFont);

  // Status label
  statusLabel = new QLabel("White to move");
  statusLabel->setAlignment(Qt::AlignCenter);
  statusLabel->setWordWrap(true);

  // Divider
  QFrame *divider = new QFrame();
  divider->setFrameShape(QFrame::HLine);
  divider->setFrameShadow(QFrame::Sunken);

  // Buttons
  QPushButton *resetBtn = new QPushButton("Reset [R]");
  QPushButton *undoBtn = new QPushButton("Undo [U]");
  QPushButton *playEngineWhite = new QPushButton("Play engine as White");
  QPushButton *playEngineBlack = new QPushButton("Play engine as Black");
  QPushButton *playHuman = new QPushButton("Human vs. Human");

  // Layout
  layout->addWidget(title);
  layout->addSpacing(8);
  layout->addWidget(statusLabel);
  layout->addWidget(divider);
  layout->addSpacing(8);
  layout->addWidget(resetBtn);
  layout->addWidget(undoBtn);
  layout->addWidget(divider);
  layout->addWidget(playHuman);
  layout->addWidget(playEngineWhite);
  layout->addWidget(playEngineBlack);
  layout->addStretch(); // pushes everything to the top.

  connect(resetBtn, &QPushButton::clicked, this, &SidePanel::onResetClicked);
  connect(undoBtn, &QPushButton::clicked, this, &SidePanel::onUndoClicked);

  // Connect play engine buttons, once engine is made.
}

void SidePanel::onResetClicked() {
  game.reset();
  emit gameStateChanged();
}

void SidePanel::onUndoClicked() {
  game.undoMove();
  emit gameStateChanged();
}

void SidePanel::refresh() {
  switch (game.getResult()) {
  case GameResult::Ongoing:
    statusLabel->setText(game.isWhiteTurn() ? "White to move"
                                            : "Black to move");
    break;
  case GameResult::WhiteWins:
    statusLabel->setText("White Wins!");
    break;
  case GameResult::BlackWins:
    statusLabel->setText("Black Wins!");
    break;
  case GameResult::Draw:
    statusLabel->setText("Draw");
    break;
  }
}
