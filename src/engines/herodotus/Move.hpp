#pragma once
#include "HerodotusEngine.hpp"
#include "Square.hpp"

struct Move {
  Color color;
  Piece piece;
  Square from;
  Square to;
  std::optional<Piece> captured;
  std::optional<Piece> promotion;
  bool isEnPassant = false;
  // It should be possible to derive information
  // like if it is a castle, double pawn push 
  // or promotion, but we will see.
};
