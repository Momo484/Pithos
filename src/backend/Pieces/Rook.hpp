#pragma once
#include "Piece.hpp"

class Rook: public Piece {
private:
    static const std::vector<std::pair<int,int>> dirs;
    bool mayCastle;

public:
    Rook(bool isWhite, Square position);

    ~Rook() override;

    std::vector<Move> getLegalMoves(const Board& board) override; 

    void setCannotCastle();

    bool canCastle();
};