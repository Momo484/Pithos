#pragma once 
#include "Piece.hpp"

class Bishop : public Piece {
private:
    static const std::vector<std::pair<int,int>> dirs;

public: 
    Bishop(bool isWhite, Square position);

    ~Bishop() override;

    std::vector<Move>getLegalMoves(const Board& board) override;
};