#pragma once

#include <vector>
#include "types/Move.hpp"

class HerodotusEngine;

namespace MoveGen {
void generatePseudoLegalMoves(HerodotusEngine &engine, std::vector<Move> &out);
}
