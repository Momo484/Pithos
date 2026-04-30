#pragma once 
#include "Piece.hpp"

class King : public Piece {
private:
    static const std::vector<std::pair<int,int>> dirs;
    bool mayCastle;

public: 
    King(bool isWhite, Square position);

    ~King() override;

    std::vector<Move>getLegalMoves(const Board& board) override;

    void setCannotCastle();

    bool canCastle();
};