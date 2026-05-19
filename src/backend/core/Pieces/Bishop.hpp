#pragma once
#include "../Square.hpp"
#include "Piece.hpp"

class Bishop : public Piece {
private:
  static const std::vector<std::pair<int, int>> dirs;

public:
  Bishop(bool isWhite);

  ~Bishop() override;

  std::vector<Move> getLegalMoves(const Board &board, Square position) override;
};
