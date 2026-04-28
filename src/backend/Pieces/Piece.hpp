#pragma once
#include "../Square.hpp" // Now Piece knows what a Square is
#include "../Move.hpp"   // Needs this for the getLegalMoves return type
#include "../Board.hpp"
#include <vector>

// Forward declaration: We promise the compiler that 'Board' exists elsewhere.
// This prevents circular dependency errors when Piece and Board look at each other.
class Board; 

class Piece {
protected:
    bool isWhite;
    char symbol; // e.g., 'P' for Pawn, 'R' for Rook, uppercase for white peices.
    Square position;

    std::vector<Move> getSlidingMoves(const Board& board, const std::vector<std::pair<int,int>>& dirs) const;
public:
    // Constructor
    Piece(bool white, char sym, Square startPos);
    
    virtual ~Piece();

    char getSymbol() const;
    bool getIsWhite() const;

    // these are pseudo legal moves, they do not account for checks.
    virtual std::vector<Move> getLegalMoves(const Board& board) = 0;

    // remember to deal with checks in this class.
};