#include "King.hpp"
#include "Rook.hpp"

using namespace std;

King::King(bool isWhite, Square position) : Piece(isWhite, (isWhite ? 'K' : 'k'), position) {
    // king can castle at the start of the game.
    mayCastle = true;
}

King::~King() {}

const std::vector<std::pair<int,int>> King::dirs = {
    {1, 0}, {-1, 0}, {0, 1}, {0, -1}, // Rook dirs
    {1, 1}, {1, -1}, {-1, 1}, {-1, -1} // Bishop dirs
};

vector<Move> King::getLegalMoves(const Board& board) {
    vector<Move> moves;
    // lets first check all the 1 steppers 
    for (const auto& dir : dirs) {
        Square moveTo = {position.x + dir.first, position.y + dir.second};
        
        // first, bound checking
        if (!board.onBoard(moveTo)) continue;

        Piece* target = board.getPieceAt(moveTo);
        if (target == nullptr) {
            // we good to move here pseuo-legally
            moves.push_back(Move(position, moveTo, MoveType::Normal, isWhite));
        } else if (target->getIsWhite() != isWhite) {
            moves.push_back(Move(position, moveTo, MoveType::Capture, target->getSymbol()));
        }
        // otherwise, we can't make this move.
    }
    
    // now lets check if we may castle

    // Castling — board is the single source of truth for rights
    CastlingRights rights = board.getCastlingRights();
    bool kingSide  = isWhite ? rights.whiteKingSide  : rights.blackKingSide;
    bool queenSide = isWhite ? rights.whiteQueenSide : rights.blackQueenSide;

    if (kingSide) {
        Square s1 = {position.x + 1, position.y};
        Square s2 = {position.x + 2, position.y};
        if (!board.getPieceAt(s1) && !board.getPieceAt(s2))
            moves.push_back(Move(position, s2, MoveType::CastleKingSide, isWhite));
    }

    if (queenSide) {
        Square s1 = {position.x - 1, position.y};
        Square s2 = {position.x - 2, position.y};
        Square s3 = {position.x - 3, position.y};
        if (!board.getPieceAt(s1) && !board.getPieceAt(s2) && !board.getPieceAt(s3))
            moves.push_back(Move(position, s2, MoveType::CastleQueenSide, isWhite));
    }

    return moves;
};



// called when a the king makes their first move, must be checked in conjuction
// if the rooks status.
void King::setCannotCastle() {
    mayCastle = false;
}

bool King::canCastle() {
    return mayCastle;
}



