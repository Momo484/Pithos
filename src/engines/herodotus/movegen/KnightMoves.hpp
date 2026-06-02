#pragma once

#include "types/Bitboard.hpp"
#include "types/Move.hpp"
#include "types/Types.hpp"
#include <vector>

namespace MoveGen {
void generateKnightMoves(Bitboard knightBB, Bitboard friendly, Color side,
                         std::vector<Move> &out);
}
