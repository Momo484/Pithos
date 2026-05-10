#include "Move.hpp"

// Constructor 1: Building a move from coordinates
Move::Move(Square fromSquare, Square toSquare, MoveType type, bool isWhite) 
    : from(fromSquare), to(toSquare), type(type), isWhite(isWhite) {
}

Move::Move(Square fromSquare, Square toSquare, MoveType type, bool isWhite, char capturedPieceSymbol, char promotionPiece) 
    : from(fromSquare), to(toSquare), type(type), isWhite(isWhite), capturedPieceSymbol(capturedPieceSymbol), promotionPiece(promotionPiece) {
}


// Getters
Square Move::getFrom() const {
    return from;
}

Square Move::getTo() const {
    return to;
}

bool Move::getIsWhite() const {
    return isWhite;
}

