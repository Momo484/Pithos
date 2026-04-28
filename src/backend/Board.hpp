#pragma once
#include "Pieces/Piece.hpp"
#include "Square.hpp"

class Piece;

class Board{
private:
    std::unique_ptr<Piece> squares[8][8];
    Square epTarget;

public:

    Board();
    ~Board();

    Piece* getPieceAt(Square sq) const;
    void setupStartingPosition();
    Square getEpTarget() const; 
    void clearEpTarget();
    void setEpTarget(Square sq);
    const static bool onBoard(Square sq);
};