#pragma once
#include "BoardMemento.hpp"
#include "Pieces/Piece.hpp"
#include "Square.hpp"
#include <memory>
#include <stack>

class Piece;

class Board{
private:
    std::unique_ptr<Piece> squares[8][8];
    Square epTarget = {-1, -1};
    CastlingRights castling {};
    std::stack<BoardMemento> history;

    int halfMoveClock = 0;
    int fullMoveClock = 0;

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
    
    bool isKingChecked(bool isWhite);
    std::vector<Move> generateAllLegalMoves(bool isWhiteTurn);

    void clearEpTarget();
    void setEpTarget(Square sq);

    std::string generateFEN();
};