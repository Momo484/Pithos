#include "Board.hpp"
#include "BoardMemento.hpp"
#include "Move.hpp"
#include "Pieces/Pawn.hpp"
#include "Pieces/Rook.hpp"
#include "Pieces/Knight.hpp"
#include "Pieces/Bishop.hpp"
#include "Pieces/Queen.hpp"
#include "Pieces/King.hpp"
#include <cctype>
#include <memory>
#include <vector>
#include <assert.h>



// -- Helpers --------------------------------------------------------------------------------------
bool Board::onBoard(Square sq) {
    return (sq.x >= 0 && sq.x <= 7 && sq.y >= 0 && sq.y <= 7);
}

Piece* Board::getPieceAt(Square sq) const {
    if (!onBoard(sq)) {
        return nullptr;
    }

    return squares[sq.y][sq.x].get();
}

void Board::clearEpTarget() {
    epTarget = {-1, -1};
}

void Board::setEpTarget(Square ep) {
    epTarget = ep;
}

// Reconstructs a piece from a captured symbol — this is what keeps the
// memento lightweight. No unique_ptr in the snapshot, just a char.
std::unique_ptr<Piece> Board::makePiece(char symbol, bool isWhite, Square pos) const {
    switch (std::tolower(symbol)) {
        case 'p': return std::make_unique<Pawn>(isWhite, pos);
        case 'r': return std::make_unique<Rook>(isWhite, pos);
        case 'n': return std::make_unique<Knight>(isWhite, pos);
        case 'b': return std::make_unique<Bishop>(isWhite, pos);
        case 'q': return std::make_unique<Queen>(isWhite, pos);
        case 'k': return std::make_unique<King>(isWhite, pos);
        default:  return nullptr;
    }
}

void Board::revokeCastlingRights(Square sq) {
    if (sq == Square{4, 0}) { castling.whiteKingSide = castling.whiteQueenSide = false; }
    if (sq == Square{4, 7}) { castling.blackKingSide = castling.blackQueenSide = false; }
    if (sq == Square{0, 0}) castling.whiteQueenSide = false;
    if (sq == Square{7, 0}) castling.whiteKingSide  = false;
    if (sq == Square{0, 7}) castling.blackQueenSide = false;
    if (sq == Square{7, 7}) castling.blackKingSide  = false;
}

// -- Setup ----------------------------------------------------------------------------------------
Board::Board() {
    setupStartingPosition();
}

void Board::setupStartingPosition() {
    
    // --- BLACK PIECES (Top of the board) ---
    
    // Rank 8 (y = 7): Major and Minor Pieces
    squares[7][0] = std::make_unique<Rook>  (false, Square{0, 7});
    squares[7][1] = std::make_unique<Knight>(false, Square{1, 7});
    squares[7][2] = std::make_unique<Bishop>(false, Square{2, 7});
    squares[7][3] = std::make_unique<Queen> (false, Square{3, 7});
    squares[7][4] = std::make_unique<King>  (false, Square{4, 7}); // King on 'e' file (x=4)
    squares[7][5] = std::make_unique<Bishop>(false, Square{5, 7});
    squares[7][6] = std::make_unique<Knight>(false, Square{6, 7});
    squares[7][7] = std::make_unique<Rook>  (false, Square{7, 7});

    // Rank 7 (y = 6): Black Pawns
    for (int i = 0; i < 8; i++) {
        squares[6][i] = std::make_unique<Pawn>(false, Square{i, 6});
    }

    // --- WHITE PIECES (Bottom of the board) ---

    // Rank 2 (y = 1): White Pawns
    for (int i = 0; i < 8; i++) {
        squares[1][i] = std::make_unique<Pawn>(true, Square{i, 1});
    }

    // Rank 1 (y = 0): Major and Minor Pieces
    squares[0][0] = std::make_unique<Rook>  (true, Square{0, 0});
    squares[0][1] = std::make_unique<Knight>(true, Square{1, 0});
    squares[0][2] = std::make_unique<Bishop>(true, Square{2, 0});
    squares[0][3] = std::make_unique<Queen> (true, Square{3, 0});
    squares[0][4] = std::make_unique<King>  (true, Square{4, 0}); // King on 'e' file (x=4)
    squares[0][5] = std::make_unique<Bishop>(true, Square{5, 0});
    squares[0][6] = std::make_unique<Knight>(true, Square{6, 0});
    squares[0][7] = std::make_unique<Rook>  (true, Square{7, 0});

    // Reset the En Passant target since it's the start of the game
    clearEpTarget();
    castling = {};

}

// -- Check Detection ------------------------------------------------------------------------------
bool Board::isKingChecked(bool isWhite) {
    // ray cast from the king 
    // first we find the king, we could keep a reference, but that seems uneccesary.
    char kingSymbol = isWhite ? 'K' : 'k';
    Square kingPos = {-1, -1};
    // findin the correct king.
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (auto* p = squares[i][j].get()) {
                if (p->getSymbol() == kingSymbol) {
                    kingPos = {j, i};
                }
            }
        }
    }

    // Opponent symbols we test against
    char oppQueen  = isWhite ? 'q' : 'Q';
    char oppRook   = isWhite ? 'r' : 'R';
    char oppBishop = isWhite ? 'b' : 'B';
    char oppKnight = isWhite ? 'n' : 'N';
    char oppPawn   = isWhite ? 'p' : 'P';
    char oppKing   = isWhite ? 'k' : 'K';  // for adjacent-king adjacency rule

    // ── Diagonals: bishops / queens ──
    for (auto [dx, dy] : {std::pair{1,1},{1,-1},{-1,1},{-1,-1}}) {
        Square sq = kingPos;
        while (true) {
            sq.x += dx; sq.y += dy;
            if (!onBoard(sq)) break;
            Piece* p = getPieceAt(sq);
            if (!p) continue;
            if (p->getIsWhite() == isWhite) break;   // friendly blocker
            char s = p->getSymbol();
            if (s == oppBishop || s == oppQueen) return true;
            break;   // any other enemy just blocks
        }
    }

    // ── Straights: rooks / queens ──
    for (auto [dx, dy] : {std::pair{1,0},{-1,0},{0,1},{0,-1}}) {
        Square sq = kingPos;
        while (true) {
            sq.x += dx; sq.y += dy;
            if (!onBoard(sq)) break;
            Piece* p = getPieceAt(sq);
            if (!p) continue;
            if (p->getIsWhite() == isWhite) break;
            char s = p->getSymbol();
            if (s == oppRook || s == oppQueen) return true;
            break;
        }
    }

    // ── Knights ──
    for (auto [dx, dy] : {std::pair{2,1},{2,-1},{-2,1},{-2,-1},
                                        {1,2},{1,-2},{-1,2},{-1,-2}}) {
        Square sq = {kingPos.x + dx, kingPos.y + dy};
        if (!onBoard(sq)) continue;
        Piece* p = getPieceAt(sq);
        if (p && p->getIsWhite() != isWhite && p->getSymbol() == oppKnight)
            return true;
    }

    // ── Pawns ──
    // White king is attacked by black pawns coming from above (dy = +1 from king)
    int pawnDir = isWhite ? 1 : -1;
    for (int dx : {-1, 1}) {
        Square sq = {kingPos.x + dx, kingPos.y + pawnDir};
        if (!onBoard(sq)) continue;
        Piece* p = getPieceAt(sq);
        if (p && p->getIsWhite() != isWhite && p->getSymbol() == oppPawn)
            return true;
    }

    // ── Adjacent king (prevents kings walking next to each other) ──
    for (auto [dx, dy] : {std::pair{1,0},{-1,0},{0,1},{0,-1},
                                        {1,1},{1,-1},{-1,1},{-1,-1}}) {
        Square sq = {kingPos.x + dx, kingPos.y + dy};
        if (!onBoard(sq)) continue;
        Piece* p = getPieceAt(sq);
        if (p && p->getIsWhite() != isWhite && p->getSymbol() == oppKing)
            return true;
    }

    return false;
}



// -- Legal move generation ------------------------------------------------------------------------
bool Board::validateMove(Move move) {
    makeMove(move);
    bool legal = !isKingChecked(move.getIsWhite());
    undoMove();
    return legal;
}

std::vector<Move> Board::pseudoToLegalMoves(std::vector<Move> moves) {
    std::vector<Move> legalMoves; 
    for (Move move : moves) {
        if (validateMove(move)) {
            legalMoves.push_back(move);
        }
    }
    return legalMoves;
}

std::vector<Move> Board::generateAllLegalMoves(bool isWhiteTurn) {
    std::vector<Move> pseudoLegalMoves;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Piece *piece = getPieceAt({i, j});
            if (piece != nullptr && piece->getIsWhite() == isWhiteTurn) {
                std::vector<Move> piecePseudoLegal = piece->getLegalMoves(*this);
                pseudoLegalMoves.insert(pseudoLegalMoves.end(), 
                    piecePseudoLegal.begin(), piecePseudoLegal.end());
            }
        }
    }
    std::vector<Move> legalMoves = pseudoToLegalMoves(pseudoLegalMoves);
    return legalMoves;
}

// -- makeMove -------------------------------------------------------------------------------------
void Board::makeMove(Move move) {
    // push the snapshot
    history.push({move, epTarget, castling});

    Square from = move.getFrom();
    Square to = move.getTo();
    epTarget = {-1, -1};

    switch (move.getType()) {
        case MoveType::Normal:
        case MoveType::Capture:
            squares[to.y][to.x] = std::move(squares[from.y][from.x]);
            squares[to.y][to.x]->setPosition(to);
            break;
        
        case MoveType::DoublePawnPush:
            squares[to.y][to.x] = std::move(squares[from.y][from.x]);
            squares[to.y][to.x]->setPosition(to);
            epTarget = {from.x, (from.y + to.y) / 2};
            break;
        case MoveType::EnPassant: {
            squares[to.y][to.x] = std::move(squares[from.y][from.x]);
            squares[to.y][to.x]->setPosition(to);
            // Remove the captured pawn — it sits on 'to.x' but at the
            // moving pawn's original rank, not the destination rank
            squares[from.y][to.x].reset();
        }

        case MoveType::CastleKingSide: {
            int r = from.y;
            // King: e→g
            squares[r][6] = std::move(squares[r][4]);
            squares[r][6]->setPosition({6, r});
            // Rook: h→f
            squares[r][5] = std::move(squares[r][7]);
            squares[r][5]->setPosition({5, r});
            break;
        }

        case MoveType::CastleQueenSide: {
            int r = from.y;
            // King: e→c
            squares[r][2] = std::move(squares[r][4]);
            squares[r][2]->setPosition({2, r});
            // Rook: a→d
            squares[r][3] = std::move(squares[r][0]);
            squares[r][3]->setPosition({3, r});
            break;
        }

        case MoveType::Promotion: {
            squares[from.y][from.x].reset();   // remove pawn
            bool isWhite = move.getIsWhite();
            squares[to.y][to.x] = makePiece(move.getPromotionPiece(), isWhite, to);
            break;
        }
    }

    revokeCastlingRights(from);
    revokeCastlingRights(to);

}

// -- undoMove -------------------------------------------------------------------------------------
void Board::undoMove() {
    assert(!history.empty() && "undoMove called with empty history");
    BoardMemento memento = std::move(history.top());
    history.pop();

    const Move& move = memento.move;
    Square from = move.getFrom();
    Square to = move.getTo();

    // retoring previous state. 
    epTarget = memento.prevEpTarget;
    castling = memento.prevCastling;

    switch (move.getType()) {
        case MoveType::Normal:
            squares[from.y][from.x] = std::move(squares[to.y][to.x]);
            squares[from.y][from.x]->setPosition(from);
            break;
        
        case MoveType::Capture: {
            squares[from.y][from.x] = std::move(squares[to.y][to.x]);
            squares[from.y][from.x]->setPosition(from);
            char sym = move.getCapturedPieceSymbol();
            bool capturedWasWhite = std::isupper(sym);
            squares[to.y][to.x] = makePiece(sym, capturedWasWhite, to);
            break;
        }

        case MoveType::DoublePawnPush:
            squares[from.y][from.x] = std::move(squares[to.y][to.x]);
            squares[from.y][from.x]->setPosition(from);
            break;

        case MoveType::EnPassant: {
            squares[from.y][from.x] = std::move(squares[to.y][to.x]);
            squares[from.y][from.x]->setPosition(from);
            // Restore the captured pawn at its original square
            bool capturedWasWhite = !move.getIsWhite();
            squares[from.y][to.x] = makePiece('P', capturedWasWhite, {to.x, from.y});
            break;
        }

        case MoveType::CastleKingSide: {
            int r = from.y;
            squares[r][4] = std::move(squares[r][6]);   // king g→e
            squares[r][4]->setPosition({4, r});
            squares[r][7] = std::move(squares[r][5]);   // rook f→h
            squares[r][7]->setPosition({7, r});
            break;
        }

        case MoveType::CastleQueenSide: {
            int r = from.y;
            squares[r][4] = std::move(squares[r][2]);   // king c→e
            squares[r][4]->setPosition({4, r});
            squares[r][0] = std::move(squares[r][3]);   // rook d→a
            squares[r][0]->setPosition({0, r});
            break;
        }

        case MoveType::Promotion: {
            // Put a pawn back, remove the promoted piece
            bool isWhite = move.getIsWhite();
            squares[from.y][from.x] = makePiece('P', isWhite, from);
            // Restore any piece that was captured on the promotion square
            char sym = move.getCapturedPieceSymbol();
            if (sym != 0) {
                bool capturedWasWhite = std::isupper(sym);
                squares[to.y][to.x] = makePiece(sym, capturedWasWhite, to);
            } else {
                squares[to.y][to.x].reset();
            }
            break;
        }
    }
}

