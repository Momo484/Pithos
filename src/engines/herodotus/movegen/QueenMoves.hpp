#pragma once

#include "types/Bitboard.hpp"
#include "types/Move.hpp"
#include "types/Types.hpp"
#include <vector>

namespace MoveGen {
void generateQueenMoves(Bitboard queenBB, Bitboard friendly, Bitboard occupancy,
                        Color side, std::vector<Move> &out);
}
