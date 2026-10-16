#pragma once

#include <optional>
#include "Types.hpp"
#include "Square.hpp"

// A fully described chess move: applying it to a position (and later undoing
// it) requires no further board lookups or chess knowledge.
struct Move {
  Color color;
  Piece piece;
  Square from;
  Square to;

  // Piece type captured, if any. For en passant the captured pawn is not on `to`.
  std::optional<Piece> captured = std::nullopt;

  // Promotion target, if any (always a pawn move; never PAWN or KING).
  std::optional<Piece> promotion = std::nullopt;

  // En passant capture: `to` is the diagonal square; the captured pawn sits
  // behind it at (from.rank, to.file).
  bool isEnPassant = false;

  // Castling: `piece` is always KING and the side (kingside/queenside) is
  // recovered from `to`. The rook movement is implicit and applied separately.
  bool isCastling = false;

  // Pawn moved two squares; the engine records the en passant target this creates.
  bool isPawnDoublePush = false;
};
