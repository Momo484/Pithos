#include "Bishop.hpp"

using namespace std;

Bishop::Bishop(bool isWhite, Square position) : Piece(isWhite, (isWhite ? 'B' : 'b'), position) {}

Bishop::~Bishop() {}

const std::vector<std::pair<int,int>> Bishop::dirs = {
    {1, 1}, {-1, 1}, {1, -1}, {-1, -1}
};

vector<Move> Bishop::getLegalMoves(const Board& board) {
    
    return getSlidingMoves(board, dirs);
}

