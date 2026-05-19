#include "Bishop.hpp"

using namespace std;

Bishop::Bishop(bool isWhite) : Piece(isWhite, (isWhite ? 'B' : 'b')) {}

Bishop::~Bishop() {}

const std::vector<std::pair<int, int>> Bishop::dirs = {
    {1, 1}, {-1, 1}, {1, -1}, {-1, -1}};

vector<Move> Bishop::getLegalMoves(const Board &board, Square position) {

  return getSlidingMoves(board, position, dirs);
}
