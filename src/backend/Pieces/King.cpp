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
            moves.push_back(Move(position, moveTo, MoveType::Normal));
        } else if (target->getIsWhite() != isWhite) {
            moves.push_back(Move(position, moveTo, MoveType::Capture, target->getSymbol()));
        }
        // otherwise, we can't make this move.
    }
    
    // now lets check if we may castle
    // ... end of your 1-stepper loop ...

    // --- CASTLING ---
    // First, check if the King has moved. If he has, we don't even need to look at the Rooks.
    if (mayCastle) {
        
        // 1. KINGSIDE CASTLE (Short Castle to the right)
        // We need the two squares to our right (x=5, x=6) to be completely empty
        Square kingside1 = {position.x + 1, position.y};
        Square kingside2 = {position.x + 2, position.y};
        Square kingsideRookSq = {position.x + 3, position.y}; // x=7

        if (board.getPieceAt(kingside1) == nullptr && board.getPieceAt(kingside2) == nullptr) {
            // There is a clear path! Now, let's interrogate the corner piece.
            Piece* cornerPiece = board.getPieceAt(kingsideRookSq);
            
            // We must cast the generic Piece* into a Rook* to access its specific canCastle() function
            // this returns nullptr if the peice is not a rook.
            Rook* kingsideRook = dynamic_cast<Rook*>(cornerPiece);
            
            // If it IS a Rook, AND it belongs to us, AND it hasn't moved...
            if (kingsideRook != nullptr && kingsideRook->getIsWhite() == isWhite && kingsideRook->canCastle()) {
                // The King's destination for Kingside castling is x=6 (the 'g' file)
                moves.push_back(Move(position, kingside2, MoveType::CastleKingSide));
            }
        }

        // 2. QUEENSIDE CASTLE (Long Castle to the left)
        // We need the THREE squares to our left (x=3, x=2, x=1) to be empty
        Square queenside1 = {position.x - 1, position.y};
        Square queenside2 = {position.x - 2, position.y};
        Square queenside3 = {position.x - 3, position.y};
        Square queensideRookSq = {position.x - 4, position.y}; // x=0

        if (board.getPieceAt(queenside1) == nullptr && 
            board.getPieceAt(queenside2) == nullptr && 
            board.getPieceAt(queenside3) == nullptr) {
            
            Piece* cornerPiece = board.getPieceAt(queensideRookSq);
            Rook* queensideRook = dynamic_cast<Rook*>(cornerPiece);
            
            if (queensideRook != nullptr && queensideRook->getIsWhite() == isWhite && queensideRook->canCastle()) {
                // The King's destination for Queenside castling is x=2 (the 'c' file)
                moves.push_back(Move(position, queenside2, MoveType::CastleQueenSide));
            }
        }
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



