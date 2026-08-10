#pragma once

#include "types/Move.hpp"
#include <vector>

class HerodotusEngine;

namespace MoveGen {
void generatePseudoLegalMoves(HerodotusEngine &engine, std::vector<Move> &out);
bool isKingChecked(HerodotusEngine &engine);
std::vector<Move> pseudoToLegalMoves(HerodotusEngine &engine,
                                     const std::vector<Move> pseudoLegalMoves);
bool validateMove(HerodotusEngine &engine, Move move);
} // namespace MoveGen
