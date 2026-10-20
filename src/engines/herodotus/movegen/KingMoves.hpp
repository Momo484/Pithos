#pragma once

#include "types/Bitboard.hpp"
#include "types/GameState.hpp"
#include "types/Move.hpp"
#include "types/Types.hpp"
#include <vector>

namespace MoveGen {

// Generates all pseudo-legal king moves for `side`: one-square moves and
// captures from the precomputed king attack table, plus castling when the
// rights are set in `state` and the squares between king and rook are clear
// in `all`. Castling is generated pseudo-legally; check safety is left to the
// validator.
void generateKingMoves(const PositionBitboards &pieces,
                       const Mailbox &mailbox, Bitboard friendly, Bitboard enemy,
                       Bitboard all, Color side, const GameState &state,
                       std::vector<Move> &out);
}
