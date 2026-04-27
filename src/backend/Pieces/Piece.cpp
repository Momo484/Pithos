#include "Piece.hpp"

using namespace std;

// Constructor implementation using a C++ "Initializer List"
Piece::Piece(bool white, char sym, Square startPos) 
            : isWhite(white), symbol(sym), position(startPos) {
}

// Destructor implementation
Piece::~Piece() {
}

// Getters
char Piece::getSymbol() const {
    return symbol;
}

bool Piece::getIsWhite() const {
    return isWhite;
}