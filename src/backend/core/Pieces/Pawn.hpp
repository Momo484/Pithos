#pragma once
#include "Piece.hpp"
#include <vector>


class Pawn : public Piece {
private:

public:
    Pawn(bool white, Square position);
    
    // Override the base destructor
    ~Pawn() override;

    // pseudo legal move generation.
    std::vector<Move> getLegalMoves(const Board& board) override; 
};