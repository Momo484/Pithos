#pragma once

#include "types/Move.hpp"
#include <vector>

class HerodotusEngine;

// Top-level move generation pipeline: pseudo-legal generation, check
// detection, and filtering down to legal moves. The functions below are
// declared as friends of HerodotusEngine so they can read its bitboards,
// mailbox and game state directly.
namespace MoveGen {

// Generates every pseudo-legal move for the engine's current position by
// calling each piece generator in turn.
void generatePseudoLegalMoves(HerodotusEngine &engine, std::vector<Move> &out);

// True if `side`'s king is currently attacked. Gathers the attacks that can
// reach the king square, then confirms the threatening piece type with the
// mailbox.
bool isKingChecked(HerodotusEngine &engine, Color side);

// Filters `pseudoLegalMoves` down to the moves that don't leave our own king
// in check, one validateMove call each.
std::vector<Move> pseudoToLegalMoves(HerodotusEngine &engine,
                                     const std::vector<Move> pseudoLegalMoves);

// True if `move` is legal: plays it out, checks the king, then undoes it.
// Castling additionally plays the transit square to rule out moving through
// check.
bool validateMove(HerodotusEngine &engine, Move move);
} // namespace MoveGen
