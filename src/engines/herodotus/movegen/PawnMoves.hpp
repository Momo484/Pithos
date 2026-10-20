#pragma once

#include "types/Bitboard.hpp"
#include "types/GameState.hpp"
#include "types/Move.hpp"
#include "types/Types.hpp"
#include <vector>

namespace MoveGen {

// Generates all pseudo-legal pawn moves for `side`: single and double pushes,
// diagonal captures, promotions (all four pieces per destination), and en
// passant. `all` is only used for the empty-square checks on pushes; captures
// record the captured piece from `mailbox`.
void generatePawnMoves(const PositionBitboards &pieces, const Mailbox &mailbox,
                       Bitboard friendly, Bitboard enemy, Bitboard all,
                       Color side, const GameState &state,
                       std::vector<Move> &out);
}
