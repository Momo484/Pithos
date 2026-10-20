#pragma once

#include "types/Bitboard.hpp"
#include "types/Move.hpp"
#include "types/Types.hpp"
#include <vector>

namespace MoveGen {

// Generates all pseudo-legal bishop moves for `side` using magic bitboard
// diagonal attacks against `friendly | enemy` occupancy. Friendly squares are
// filtered out of the destinations; captures record the captured piece from
// `mailbox`.
void generateBishopMoves(const PositionBitboards &pieces,
                         const Mailbox &mailbox, Bitboard friendly,
                         Bitboard enemy, Color side, std::vector<Move> &out);
}
