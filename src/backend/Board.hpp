#pragma once
#include "BoardMemento.hpp"
#include "Pieces/Piece.hpp"
#include "Square.hpp"
#include <memory>
#include <stack>
#include <string>
#include <unordered_map>

class Piece;

class Board{
private:
    std::unique_ptr<Piece> squares[8][8];
    Square epTarget = {-1, -1};
    CastlingRights castling {};
    std::stack<BoardMemento> history;

    int halfMoveClock = 0;
    int fullMoveClock = 1;
    // This keeps the count of every board state, so in the case that
    // the board state repeats 3 times, before it is cleared, we can call threefold repitition.
    // A board state cannot be repeated after a pawn move or capture, so no need to encapsulate it
    // in the memento. it is represented by the first field on the FEN.
    std::unordered_map<std::string, int> boardStateCount;

    std::unique_ptr<Piece> makePiece(char symbol, bool isWhite, Square pos) const;
    std::vector<Move> pseudoToLegalMoves(std::vector<Move> moves);

    void revokeCastlingRights(Square sq);
    
public:

    Board();
    ~Board() = default;

    void setupStartingPosition();
    Piece* getPieceAt(Square sq) const;
    static bool onBoard(Square sq);


    Square getEpTarget() const {return epTarget; };
    CastlingRights getCastlingRights() const {return castling; }
    
    bool validateMove(Move move);
    void makeMove(Move move);
    void undoMove();
    
    const bool isKingChecked(bool isWhite);
    std::vector<Move> generateAllLegalMoves(bool isWhiteTurn);
    bool checkThreeFoldRepitition();

    void clearEpTarget();
    void setEpTarget(Square sq);

    std::string generateFEN();
};