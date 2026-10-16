#pragma once

#include "Bitboard.hpp"
#include "Square.hpp"
#include <array>

// Core enums and container aliases used across the engine.

enum Piece {
  PAWN = 0,
  KNIGHT = 1,
  BISHOP = 2,
  ROOK = 3,
  QUEEN = 4,
  KING = 5,
  NUM_PIECES = 6
};

enum Color {
  WHITE = 0,
  BLACK = 1,
  NUM_COLORS = 2
};

// One bitboard per piece type.
using PieceBitboards = std::array<Bitboard, Piece::NUM_PIECES>;

// One set of piece bitboards per colour.
using PositionBitboards = std::array<PieceBitboards, Color::NUM_COLORS>;

// Square index -> piece type; NUM_PIECES means the square is empty.
using Mailbox = std::array<Piece, Square::NUM_SQUARES>;
