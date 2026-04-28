#include "Queen.hpp"

using namespace std;

Queen::Queen(bool isWhite, Square position) : Piece(isWhite, (isWhite ? 'B' : 'b'), position) {}

Queen::~Queen() {}

const std::vector<std::pair<int,int>> Queen::dirs = {
    {1, 0}, {-1, 0}, {0, 1}, {0, -1}, // Rook dirs
    {1, 1}, {1, -1}, {-1, 1}, {-1, -1} // Bishop dirs
};

vector<Move> Queen::getLegalMoves(const Board& board) {
    return getSlidingMoves(board, dirs);
}

