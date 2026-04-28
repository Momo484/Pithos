#pragma once
#include "Piece.hpp"
#include <vector>


class Knight : public Piece {
private:
    std::vector<std::pair<int,int>> moveOffsets 
        = {{2, 1}, {2, -1},
            {-2, 1}, {-2, 1}, 
            {1, 2}, {1, -2},
            {-1, 2}, {-1, -2}};

public:
    Knight(bool white, Square position);
    
    // Override the base destructor
    ~Knight() override;

    // pseudo legal move generation.
    std::vector<Move> getLegalMoves(const Board& board) override; 

};