#include "Piece.hpp"
#include "../Board.hpp"

using namespace std;

// Constructor implementation using a C++ "Initializer List"
Piece::Piece(bool white, char sym, Square startPos) 
            : isWhite(white), symbol(sym), position(startPos) {
}

// Destructor implementation
Piece::~Piece() {
}

// Getters
char Piece::getSymbol() const {
    return symbol;
}

bool Piece::getIsWhite() const {
    return isWhite;
}

std::vector<Move> Piece::getSlidingMoves(const Board& board, const std::vector<std::pair<int,int>>& dirs) const {
    std::vector<Move> moves;

    for (const auto& dir : dirs) {
        Square moveTo = position;
        while (true) {
            moveTo.x += dir.first;
            moveTo.y += dir.second;
            
            if (!board.onBoard(moveTo)) break;
            
            Piece* target = board.getPieceAt(moveTo);
            
            if (target != nullptr && isWhite == target->getIsWhite()) break;

            if (target != nullptr) {
                moves.push_back(Move(position, moveTo, MoveType::Capture, target->getSymbol()));
                break; 
            } else {
                moves.push_back(Move(position, moveTo, MoveType::Normal));
            }
        }
    }
    return moves;
}