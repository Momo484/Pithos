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
    char capturedPieceSymbol;

public:
    // Constructors now take the Square struct
    Move(Square fromSquare, Square toSquare, MoveType type);
    Move(Square fromSquare, Square toSquare, MoveType type, char capturedPieceSymbol);
    Move(std::string uciString);

    // Getters return the Square struct
    Square getFrom() const;
    Square getTo() const;
    
    const std::string& getUci() const; 
};