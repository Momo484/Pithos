#pragma once

#include "types/Bitboard.hpp"
#include "types/GameState.hpp"
#include "types/Move.hpp"
#include "types/Types.hpp"
#include <vector>

namespace MoveGen {
void generateKingMoves(Bitboard kingBB, Bitboard friendly, Bitboard all,
                       Color side, const GameState &state,
                       std::vector<Move> &out);
}
