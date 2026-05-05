#include "Move.hpp"

// Constructor 1: Building a move from coordinates
Move::Move(Square fromSquare, Square toSquare, MoveType type, bool isWhite) 
    : from(fromSquare), to(toSquare), type(type), isWhite(isWhite) {
    
    // (Later, you will write a helper function here that takes the two 
    // Square structs and automatically builds the "e2e4" uci string)
}

Move::Move(Square fromSquare, Square toSquare, MoveType type, char capturedPieceSymbol, bool isWhite) 
    : from(fromSquare), to(toSquare), type(type), capturedPieceSymbol(capturedPieceSymbol), isWhite(isWhite) {
}

// Constructor 2: Building a move from a UCI string
Move::Move(std::string uciString) 
    : uci(uciString) {
    
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

const std::string& Move::getUci() const {
    return uci;
}

