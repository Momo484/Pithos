#pragma once
#include "Square.hpp"
#include <cstdint>

class ZobristHash {
private:
  static bool initialised;
  static constexpr int NUM_PIECES = 12;
  static constexpr int NUM_SQUARES = 64;

  static uint64_t pieceSquareTable[NUM_SQUARES][NUM_PIECES];
  static uint64_t whiteToMove;
  static uint64_t castlingRights[16];
  static uint64_t enPassantFiles[8];

  static int getPieceIndex(char symbol);

public:
  static void initialise();

  static uint64_t getPieceHash(char symbol, Square sq);
  static uint64_t getCastlingHash(bool whiteKingSide, bool whiteQueenSide,
                                  bool blackKingSide, bool blackQueenSide);
  static uint64_t getEnPassantHash(Square epTarget);
  static uint64_t getToMoveHash(bool isWhite);
};
