#pragma once
#include "Square.hpp"

enum class MoveType {
    Normal,
    Capture,
    DoublePawnPush,
    EnPassant,
    CastleKingSide,
    CastleQueenSide,
    Promotion,
    PromotionCapture
};

class Move {
private:
    Square from;
    Square to;
    MoveType type;
    char capturedPieceSymbol = 0;
    char promotionPiece = 'q';   // only used for Promotion
    bool isWhite;

public:
    Move(Square from, Square to, MoveType type, bool isWhite);
    Move(Square from, Square to, MoveType type, bool isWhite, char capturedPieceSymbol, char promotionPiece);


    Square getFrom() const;
    Square getTo() const;
    bool getIsWhite() const;
    MoveType getType() const;
    char getCapturedPieceSymbol() const;
    char getPromotionPiece()const;
};