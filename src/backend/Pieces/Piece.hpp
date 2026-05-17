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

    /**
     * @brief Generates the list of sliding moves ray case in the given directions, checking for 
     * pieces in the way, this is re-used in the Rook, Bishop and Queen Pieces
     * @return A list of pseudo-legal moves in the given directions.
     */
    std::vector<Move> getSlidingMoves(const Board& board, const std::vector<std::pair<int,int>>& dirs) const;
public:
    // Constructor
    Piece(bool white, char sym, Square startPos);
    
    virtual ~Piece();

    char getSymbol() const;
    bool getIsWhite() const;

    /**
     * @brief Generates a naive list of pseudo-legal moves for the chess piece, the list of moves 
     * generated ensure that the peice moves in a legal movement pattern, and does not traverse over
     * other pieces for ray cast movement pieces, however they do not consider if the move put their 
     * King in Check, making them pseudo-legal.
     * @return A list of pseudo-legal Move objects. 
     */
    virtual std::vector<Move> getLegalMoves(const Board& board) = 0;

    void setPosition(Square newSquare);
};