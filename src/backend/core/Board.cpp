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
#include <cstddef>
#include <memory>
#include <assert.h>



// -- Helpers --------------------------------------------------------------------------------------
bool Board::onBoard(Square sq) {
    return (sq.x >= 0 && sq.x <= 7 && sq.y >= 0 && sq.y <= 7);
}

Piece* Board::getPieceAt(Square sq) const {
    if (!onBoard(sq)) {
        return nullptr;
    }

    char piece = squares[sq.y][sq.x];
    if (piece == ' ') return nullptr;

    int idx = -1;
    switch (std::tolower(piece)) {
        case 'p': idx = std::isupper(piece) ? 0 : 6; break;
        case 'n': idx = std::isupper(piece) ? 1 : 7; break;
        case 'b': idx = std::isupper(piece) ? 2 : 8; break;
        case 'r': idx = std::isupper(piece) ? 3 : 9; break;
        case 'q': idx = std::isupper(piece) ? 4 : 10; break;
        case 'k': idx = std::isupper(piece) ? 5 : 11; break;
    }

    return idx >= 0 ? pieceSingletons[idx].get() : nullptr;
}

void Board::clearEpTarget() {
    epTarget = {-1, -1};
}

void Board::setEpTarget(Square ep) {
    epTarget = ep;
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
    
    // Initialize singleton piece objects
    pieceSingletons[0]  = std::make_unique<Pawn>(true);
    pieceSingletons[1]  = std::make_unique<Knight>(true);
    pieceSingletons[2]  = std::make_unique<Bishop>(true);
    pieceSingletons[3]  = std::make_unique<Rook>(true);
    pieceSingletons[4]  = std::make_unique<Queen>(true);
    pieceSingletons[5]  = std::make_unique<King>(true);
    pieceSingletons[6]  = std::make_unique<Pawn>(false);
    pieceSingletons[7]  = std::make_unique<Knight>(false);
    pieceSingletons[8]  = std::make_unique<Bishop>(false);
    pieceSingletons[9]  = std::make_unique<Rook>(false);
    pieceSingletons[10] = std::make_unique<Queen>(false);
    pieceSingletons[11] = std::make_unique<King>(false);

    setupStartingPosition();
}

void Board::setupStartingPosition() {
    // Clear the board
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            squares[i][j] = ' ';
        }
    }

    // --- BLACK PIECES (Top of the board) ---
    
    // Rank 8 (y = 7): Major and Minor Pieces
    squares[7][0] = 'r';
    squares[7][1] = 'n';
    squares[7][2] = 'b';
    squares[7][3] = 'q';
    squares[7][4] = 'k';
    squares[7][5] = 'b';
    squares[7][6] = 'n';
    squares[7][7] = 'r';

    // Rank 7 (y = 6): Black Pawns
    for (int i = 0; i < 8; i++) {
        squares[6][i] = 'p';
    }

    blackKingPos = {4, 7};

    // --- WHITE PIECES (Bottom of the board) ---

    // Rank 2 (y = 1): White Pawns
    for (int i = 0; i < 8; i++) {
        squares[1][i] = 'P';
    }

    // Rank 1 (y = 0): Major and Minor Pieces
    squares[0][0] = 'R';
    squares[0][1] = 'N';
    squares[0][2] = 'B';
    squares[0][3] = 'Q';
    squares[0][4] = 'K';
    squares[0][5] = 'B';
    squares[0][6] = 'N';
    squares[0][7] = 'R';

    whiteKingPos = {4, 0};

    // Reset the En Passant target since it's the start of the game
    clearEpTarget();
    castling = {};
}

// -- Check Detection ------------------------------------------------------------------------------
const bool Board::isKingChecked(bool isWhite) {
    // ray cast from the king 
    // first we find the king, we could keep a reference, but that seems uneccesary.

    Square kingPos = isWhite ? whiteKingPos : blackKingPos;

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

bool Board::checkThreeFoldRepitition() {
    for (const auto& [key, count] : boardStateCount) {
        if (count >= 3) {
            return true;
        }
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

std::vector<Move> Board::pseudoToLegalMoves(const std::vector<Move> moves) {
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
            Piece *piece = getPieceAt({j, i});
            if (piece != nullptr && piece->getIsWhite() == isWhiteTurn) {
                std::vector<Move> piecePseudoLegal = piece->getLegalMoves(*this, {j, i});
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
    history.push({move, epTarget, castling, halfMoveClock});

    Square from = move.getFrom();
    Square to = move.getTo();
    char movingPiece = squares[from.y][from.x];
    char capturedPiece = squares[to.y][to.x];
    epTarget = {-1, -1};

    if (!move.getIsWhite()) {
        // we increment fullMoveClock on black turns.
        fullMoveClock++;
    }

     if (std::tolower(movingPiece) == 'k') {
        if (move.getIsWhite()) {
            whiteKingPos = to;
        } else {
            blackKingPos = to;
        }
    }

    switch (move.getType()) {
        case MoveType::Normal: {
            if (std::tolower(movingPiece) == 'p') {
                // reset halfMoveClock on pawn move. 
                halfMoveClock = 0;
            }
            squares[to.y][to.x] = movingPiece;
            squares[from.y][from.x] = ' ';
            break;
        }
        case MoveType::Capture:
            squares[to.y][to.x] = movingPiece;
            squares[from.y][from.x] = ' ';
            // reset halfMoveClock on capture.
            halfMoveClock = 0;
            break;
        
        case MoveType::DoublePawnPush:
            squares[to.y][to.x] = movingPiece;
            squares[from.y][from.x] = ' ';
            epTarget = {from.x, (from.y + to.y) / 2};
            break;
        case MoveType::EnPassant: {
            squares[to.y][to.x] = movingPiece;
            squares[from.y][from.x] = ' ';
            // Remove the captured pawn — it sits on 'to.x' but at the
            // moving pawn's original rank, not the destination rank
            squares[from.y][to.x] = ' ';
            halfMoveClock = 0;
            break;
        }

        case MoveType::CastleKingSide: {
            int r = from.y;
            // King: e→g
            squares[r][6] = movingPiece;
            squares[r][4] = ' ';
            // Rook: h→f
            char rook = squares[r][7];
            squares[r][5] = rook;
            squares[r][7] = ' ';
            break;
        }

        case MoveType::CastleQueenSide: {
            int r = from.y;
            // King: e→c
            squares[r][2] = movingPiece;
            squares[r][4] = ' ';
            // Rook: a→d
            char rook = squares[r][0];
            squares[r][3] = rook;
            squares[r][0] = ' ';
            break;
        }

        case MoveType::Promotion: {
            char promotionPiece = move.getPromotionPiece();
            squares[to.y][to.x] = promotionPiece;
            squares[from.y][from.x] = ' ';
            halfMoveClock = 0;
            break;
        }
    }

    revokeCastlingRights(from);
    revokeCastlingRights(to);
    std::string FEN = generateFEN();
    std::string key = FEN.substr(0, FEN.rfind(' '));  // strip halfmove
    key = key.substr(0, key.rfind(' '));               // strip fullmove
    boardStateCount[key]++;

}

// -- undoMove -------------------------------------------------------------------------------------
void Board::undoMove() {
    assert(!history.empty() && "undoMove called with empty history");
    BoardMemento memento = std::move(history.top());
    history.pop();

    std::string FEN = generateFEN();
    std::string key = FEN.substr(0, FEN.rfind(' '));  // strip halfmove
    key = key.substr(0, key.rfind(' '));               // strip fullmove
    boardStateCount[key]--;

    const Move& move = memento.move;
    Square from = move.getFrom();
    Square to = move.getTo();

    // retoring previous state. 
    epTarget = memento.prevEpTarget;
    castling = memento.prevCastling;
    halfMoveClock = memento.halfMoveClock;

    if (!move.getIsWhite()) {
        // we decrement our fullMoveClock since we undid a black move
        fullMoveClock--;
    }

    // update the king position
    char piece = squares[to.y][to.x];
    if (std::tolower(piece) == 'k') {
        if (move.getIsWhite()) {
            whiteKingPos = from;
        } else {
            blackKingPos = from;
        }
    }

    switch (move.getType()) {
        case MoveType::Normal:
            squares[from.y][from.x] = squares[to.y][to.x];
            squares[to.y][to.x] = ' ';
            break;
        
        case MoveType::Capture: {
            char capturedPiece = move.getCapturedPieceSymbol();
            squares[from.y][from.x] = piece;
            squares[to.y][to.x] = capturedPiece;
            break;
        }

        case MoveType::DoublePawnPush:
            squares[from.y][from.x] = piece;
            squares[to.y][to.x] = ' ';
            break;

        case MoveType::EnPassant: {
            squares[from.y][from.x] = piece;
            squares[to.y][to.x] = ' ';
            char capturedPawn = !move.getIsWhite() ? 'P' : 'p';
            squares[from.y][to.x] = capturedPawn;
            break;
        }

        case MoveType::CastleKingSide: {
            int r = from.y;
            squares[r][4] = piece;
            squares[r][6] = ' ';
            char rook = squares[r][5];
            squares[r][7] = rook;
            squares[r][5] = ' ';
            break;
        }

        case MoveType::CastleQueenSide: {
            int r = from.y;
            squares[r][4] = piece;
            squares[r][2] = ' ';
            char rook = squares[r][3];
            squares[r][0] = rook;
            squares[r][3] = ' ';
            break;
        }

        case MoveType::Promotion: {
            char pawnPiece = move.getIsWhite() ? 'P' : 'p';
            squares[from.y][from.x] = pawnPiece;
            char capturedPiece = move.getCapturedPieceSymbol();
            if (capturedPiece != 0) {
                squares[to.y][to.x] = capturedPiece;
            } else {
                squares[to.y][to.x] = ' ';
            }
            break;
        }
    }
}

// -- FEN generation -------------------------------------------------------------------------------
std::string Board::generateFEN() {
    std::string FEN = "";

    // First Field: piece position
    for (int rank = 7; rank >= 0; rank--) {
        int empty = 0;
        for (int file = 0; file < 8; file++) {
            char current = squares[rank][file];
            if (current == ' ') {
                empty++;
            } else {
                if (empty != 0) {
                    FEN += std::to_string(empty);
                }
                FEN += current;
            }
        }
        if (empty != 0) {
            FEN += std::to_string(empty);
        }
        if (rank != 0) {
            FEN += '/';
        }
    }
    FEN += " ";

    // Second Field: Active Colours 
    bool whiteTurn = true;
    if (!history.empty()) {
        const BoardMemento& memento = history.top();
        whiteTurn = !memento.move.getIsWhite();
    }
    FEN += whiteTurn ? "w" : "b";
    FEN += " ";

    // Third Field: Castling Rights
    std::string castleStr = "";
    if (castling.whiteKingSide)  castleStr += 'K';
    if (castling.whiteQueenSide) castleStr += 'Q';
    if (castling.blackKingSide)  castleStr += 'k';
    if (castling.blackQueenSide) castleStr += 'q';
    FEN += castleStr.empty() ? "-" : castleStr;
    FEN += " ";

    // Fourth Field: En Passant
    if (epTarget.x == -1 || epTarget.y == -1) {
        FEN += "-";
    } else {
        FEN += static_cast<char>('a' + epTarget.x);
        FEN += static_cast<char>('1' + epTarget.y);
    }
    FEN += " ";

    // Fifth Field: Half Move Clock
    FEN += std::to_string(halfMoveClock);
    FEN += " ";

    // Sixth Field: Full move clock
    FEN += std::to_string(fullMoveClock);

    return FEN;
}

