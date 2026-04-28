#include "Board.hpp"

Board::Board() {
    // TODO: Initialize board with pieces later
}

Board::~Board() {
}

Piece* Board::getPieceAt(Square sq) const {
    // TODO: Actually look up the piece. Returning nullptr for now so it compiles.
    return nullptr; 
}

void Board::setupStartingPosition() {
    // TODO
}

const bool Board::onBoard(Square sq) {
    return (sq.x >= 0 && sq.x <= 7 && sq.y >= 0 && sq.y <= 7);
}