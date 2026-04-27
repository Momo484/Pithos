#include "Move.hpp"

// Constructor 1: Building a move from coordinates
Move::Move(Square fromSquare, Square toSquare, MoveType type) 
    : from(fromSquare), to(toSquare), type(type) {
    
    // (Later, you will write a helper function here that takes the two 
    // Square structs and automatically builds the "e2e4" uci string)
}

Move::Move(Square fromSquare, Square toSquare, MoveType type, char capturedPieceSymbol) 
    : from(fromSquare), to(toSquare), type(type), capturedPieceSymbol(capturedPieceSymbol) {
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

const std::string& Move::getUci() const {
    return uci;
}