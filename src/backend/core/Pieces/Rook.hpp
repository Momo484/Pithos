#pragma once
#include "Piece.hpp"

class Rook: public Piece {
private:
    static const std::vector<std::pair<int,int>> dirs;

public:
    Rook(bool isWhite);

    ~Rook() override;

    std::vector<Move> getLegalMoves(const Board& board, Square position) override; 
};