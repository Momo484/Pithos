#pragma once
#include "Piece.hpp"
#include <vector>


class Knight : public Piece {
private:
    static const std::vector<std::pair<int,int>> moveOffsets;

public:
    Knight(bool white, Square position);
    
    // Override the base destructor
    ~Knight() override;

    // pseudo legal move generation.
    std::vector<Move> getLegalMoves(const Board& board) override; 

};