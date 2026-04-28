#include "Rook.hpp"

using namespace std;

Rook::Rook(bool isWhite, Square position) : Piece(isWhite, (isWhite ? 'R' : 'r'), position) {}

Rook::~Rook() {}

const std::vector<std::pair<int,int>> Rook::dirs = {
    {1, 0}, {-1, 0}, {0, 1}, {0, -1}
};

vector<Move> Rook::getLegalMoves(const Board& board) {
    // for each direction keep moving that way until the first move that 
    // is not legal, we leave castelling logic to the king. 

    vector<Move> moves;

    for (const auto& dir : dirs) {
        Square moveTo = position;
        while (true) {
            moveTo.x += dir.first;
            moveTo.y += dir.second;
            // easy if not on board, then we can't continue in this direction.
            if (!board.onBoard(moveTo)) break;
            // now we check the target square for peice.
            Piece* target = board.getPieceAt(moveTo);
            // friendly fire!
            if (target != nullptr && isWhite == target->getIsWhite()) break;

            if (target != nullptr) {
                moves.push_back(Move(position, moveTo, MoveType::Capture, target->getSymbol()));
                // we cant go further in this direction if we take in this direction.
                break;
            } else {
                moves.push_back(Move(position, moveTo, MoveType::Normal));
            }
        }
    }

    return moves;
}

