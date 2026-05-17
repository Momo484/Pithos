#include "Knight.hpp"

using namespace std;


// h for horsey!! :(( no allowed -> n for knight?
Knight::Knight(bool white, Square position) : Piece(white, (white ? 'N' : 'n'), position) {}

Knight::~Knight() {
    // idk what a destructor is meant to look like yet.
}

const std::vector<std::pair<int,int>> Knight::moveOffsets = {
    {2, 1}, {2, -1},
    {-2, 1}, {-2, -1},
    {1, 2}, {1, -2},
    {-1, 2}, {-1, -2}
};

// remember this detects pseudo legal moves, larger scales checks, like checking pins, and checks
// are determined by the board. 
vector<Move> Knight::getLegalMoves(const Board& board) {
    vector<Move> moves; 

    for (const auto& offset : moveOffsets) {
        // we check every move offset using L rule from the starting square 
        Square moveTo = {position.x + offset.first, position.y + offset.second};
        // first we check if it is bounds 
        if (!board.onBoard(moveTo)) {
            // just skip this possibility.
            continue;
        }
        // now we check if the square we moving to is friendly
        Piece* target = board.getPieceAt(moveTo);
        if (target != nullptr && isWhite == target->getIsWhite()) {
            // the peice is same colour, cant move there
            continue;
        }
        // no we just check if it was a capture 
        if (target != nullptr) {
            // it is a capture 
            moves.push_back(Move(position, moveTo, MoveType::Capture, isWhite, target->getSymbol(), ' '));
        } else {
            // its a normal psuedo legal move.
            moves.push_back(Move(position, moveTo, MoveType::Normal, isWhite));
        }
    }

    return moves;
}