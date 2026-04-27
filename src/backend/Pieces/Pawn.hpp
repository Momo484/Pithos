#pragma once
#include "Piece.hpp"
#include <vector>


// We don't need to forward declare Board here if Piece.hpp already did it, 
// but it doesn't hurt.

class Pawn : public Piece { // Added 'public'
private:

public:
    // Constructor only needs 'white'. We know the symbol is always 'P'.
    Pawn(bool white, Square position);
    
    // Override the base destructor
    ~Pawn() override;

    // Provide the specific move logic for the Pawn
    std::vector<Move> getLegalMoves(const Board& board) override; 

};