#include "Rook.hpp"

using namespace std;

Rook::Rook(bool isWhite, Square position) : Piece(isWhite, (isWhite ? 'R' : 'r'), position) {
}

Rook::~Rook() {}

const std::vector<std::pair<int,int>> Rook::dirs = {
    {1, 0}, {-1, 0}, {0, 1}, {0, -1}
};

vector<Move> Rook::getLegalMoves(const Board& board) {
    // for each direction keep moving that way until the first move that 
    // is not legal, we leave castelling logic to the king. 

    return getSlidingMoves(board, dirs);
}
