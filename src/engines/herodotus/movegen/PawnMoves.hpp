#pragma once

#include "types/Bitboard.hpp"
#include "types/GameState.hpp"
#include "types/Move.hpp"
#include "types/Types.hpp"
#include <vector>

namespace MoveGen {
void generatePawnMoves(const Bitboard (&pieces)[2][6],
                       const Piece (&mailbox)[64], Bitboard friendly,
                       Bitboard enemy, Bitboard all, Color side,
                       const GameState &state, std::vector<Move> &out);
}
