#pragma once
#include <string>
#include "Square.hpp"

enum class MoveType {
    Normal,
    Capture,
    DoublePawnPush,
    EnPassant,
    CastleKingSide,
    CastleQueenSide,
    Promotion
};

class Move {
private:
    Square from;
    Square to;
    std::string uci;
    MoveType type;
    char capturedPieceSymbol = 0;
    char promotionPiece = 'q';   // only used for Promotion
    bool isWhite;

public:
    Move(Square from, Square to, MoveType type, bool isWhite);
    Move(Square from, Square to, MoveType type, char capturedPieceSymbol, bool isWhite);
    Move(Square from, Square to, MoveType type, bool isWhite, char promotionPiece);  // promotion
    Move(std::string uciString);

    Square getFrom() const;
    Square getTo() const;
    bool getIsWhite() const;
    MoveType getType() const;
    char getCapturedPieceSymbol() const;
    char getPromotionPiece()const;
    const std::string& getUci() const;
};