#include "Pawn.hpp"
#include <cstddef>

using namespace std;

Pawn::Pawn(bool white, Square position) : Piece(white, (white ? 'P' : 'p'), position) {
}

Pawn::~Pawn() {
    // Destructor
}

vector<Move> Pawn::getLegalMoves(const Board& board) {
    vector<Move> moves;

    // 1. Setup our multipliers and ranks based on color
    int dir = isWhite ? 1 : -1;
    int epRank = isWhite ? 4 : 3; // The Y-index where En Passant is legally possible
    int leapRank = isWhite ? 1 : 6;
    int promotionRank = isWhite ? 7 : 0;

    // --- FORWARD PUSH ---
    Square forward = {position.x, position.y + dir};
    bool canMoveForward = false;
    
    // Bounds check to ensure we don't march off the board
    if (board.onBoard(forward) && board.getPieceAt(forward) == nullptr) {
        if (forward.y == promotionRank) {
            moves.push_back(Move(position, forward, MoveType::Promotion, isWhite, 0, isWhite ? 'Q' : 'q'));
            moves.push_back(Move(position, forward, MoveType::Promotion, isWhite, 0, isWhite ? 'N' : 'n'));
            moves.push_back(Move(position, forward, MoveType::Promotion, isWhite, 0, isWhite ? 'R' : 'r'));
            moves.push_back(Move(position, forward, MoveType::Promotion, isWhite, 0, isWhite ? 'B' : 'b'));
        } else {
            moves.push_back(Move(position, forward, MoveType::Normal, isWhite));
        }
        canMoveForward = true;
    }

    // --- DOUBLE PUSH (LEAP) ---
    Square leap = {position.x, position.y + (2 * dir)};
    if (canMoveForward && position.y == leapRank && board.onBoard(leap) && board.getPieceAt(leap) == nullptr) {
        moves.push_back(Move(position, leap, MoveType::DoublePawnPush, isWhite));
    }

    // --- DIAGONAL CAPTURES ---
    // Left Diagonal
    Square leftDiag = {position.x - 1, position.y + dir};
    if (board.onBoard(leftDiag)) {
        Piece* target = board.getPieceAt(leftDiag);
        // Ensure there is a piece AND it belongs to the enemy
        if (target != nullptr && target->getIsWhite() != this->isWhite) {
            if (leftDiag.y == promotionRank) {
                moves.push_back(Move(position, leftDiag, MoveType::Promotion, isWhite, target->getSymbol(), isWhite ? 'Q' : 'q'));
                moves.push_back(Move(position, leftDiag, MoveType::Promotion, isWhite, target->getSymbol(), isWhite ? 'N' : 'n'));
                moves.push_back(Move(position, leftDiag, MoveType::Promotion, isWhite, target->getSymbol(), isWhite ? 'R' : 'r'));
                moves.push_back(Move(position, leftDiag, MoveType::Promotion, isWhite, target->getSymbol(), isWhite ? 'B' : 'b'));
            }
            moves.push_back(Move(position, leftDiag, MoveType::Capture, isWhite, target->getSymbol(), ' '));
        }
    }

    // Right Diagonal
    Square rightDiag = {position.x + 1, position.y + dir};
    if (board.onBoard(rightDiag)) {
        Piece* target = board.getPieceAt(rightDiag);
        if (target != nullptr && target->getIsWhite() != this->isWhite) {
            if (rightDiag.y == promotionRank) {
                moves.push_back(Move(position, rightDiag, MoveType::Promotion, isWhite, target->getSymbol(), isWhite ? 'Q' : 'q'));
                moves.push_back(Move(position, rightDiag, MoveType::Promotion, isWhite, target->getSymbol(), isWhite ? 'N' : 'n'));
                moves.push_back(Move(position, rightDiag, MoveType::Promotion, isWhite, target->getSymbol(), isWhite ? 'R' : 'r'));
                moves.push_back(Move(position, rightDiag, MoveType::Promotion, isWhite, target->getSymbol(), isWhite ? 'B' : 'b'));
            }
            moves.push_back(Move(position, rightDiag, MoveType::Capture, isWhite, target->getSymbol(), ' '));
        }
    }

 // --- EN PASSANT ---
    Square currentEpTarget = board.getEpTarget();

    // Only allow EP if we are actually on the correct rank AND a target exists
    if (position.y == epRank && currentEpTarget.x != -1) {
        if (leftDiag == currentEpTarget) {
            // Note: En Passant ALWAYS captures a Pawn, so you can safely pass 'P' or 'p' here
            // if your Move constructor requires the captured piece symbol.
            moves.push_back(Move(position, leftDiag, MoveType::EnPassant, isWhite, isWhite ? 'p' : 'P', ' ')); 
        }
        
        if (rightDiag == currentEpTarget) {
            moves.push_back(Move(position, rightDiag, MoveType::EnPassant, isWhite, isWhite ? 'p' : 'P', ' '));
        }
    }

    return moves;
}