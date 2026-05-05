#pragma once
#include "Move.hpp"
#include "Square.hpp"

struct CastlingRights {
    bool whiteKingSide  = true;
    bool whiteQueenSide = true;
    bool blackKingSide  = true;
    bool blackQueenSide = true;
};

// Everything needed to fully reverse one makeMove call.
// Board owns these on its internal stack — nothing outside Board sees them.
struct BoardMemento {
    Move move;
    Square prevEpTarget;
    CastlingRights prevCastling;
    // No captured piece pointer needed — we reconstruct from capturedPieceSymbol
};