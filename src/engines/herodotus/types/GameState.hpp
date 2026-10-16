#pragma once

#include "Square.hpp"
#include "Types.hpp"
#include <cstdint>
#include <optional>

// Position state that can't be derived from piece locations alone.
struct GameState {
  // Side to move.
  Color activeColor = Color::WHITE;

  // Castling rights bitmask (1 = right still available):
  // bit 0 white kingside, bit 1 white queenside, bit 2 black kingside,
  // bit 3 black queenside.
  enum CastleBit : uint8_t {
    WHITE_KINGSIDE_CASTLE = 0x1,
    WHITE_QUEENSIDE_CASTLE = 0x2,
    BLACK_KINGSIDE_CASTLE = 0x4,
    BLACK_QUEENSIDE_CASTLE = 0x8,
  };

  uint8_t castlingRights = 0b00001111;

  // Plies since the last capture or pawn move (draw at 100, the 50-move rule).
  int halfMoveClock = 0;

  // Full move number from 1, incremented after Black's moves.
  int fullMoveNumber = 1;

  // En passant capture target (the square behind a pawn that just pushed two),
  // if one is available this ply.
  std::optional<Square> enPassant = std::nullopt;
};
