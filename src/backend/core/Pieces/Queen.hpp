#pragma once
#include "Piece.hpp"

class Queen : public Piece {
private:
  static const std::vector<std::pair<int, int>> dirs;

public:
  Queen(bool isWhite);

  ~Queen() override;

  std::vector<Move> getLegalMoves(const Board &board, Square position) override;
};
