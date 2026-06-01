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

  /**
   * @brief returns the index of the piece int [0, NUM_PIECES).
   * Pawn: white -> 0, black -> 6
   * Knight: white -> 1, black -> 7
   * Bishop: white -> 2, black -> 8
   * Rook: white -> 3, black -> 9
   * Queen: white -> 4, black -> 10
   * King: white -> 5, black -> 11
   */
  static int getPieceIndex(char symbol);

public:
  /**
  * @brief initialises the hash values for each piece, the castling rights 
  * possiblities and enPassant files witha random uint16_t
  */
  static void initialise();

  // Getters for each of the possible hashes for XOR-ing 
  static uint64_t getPieceHash(char symbol, Square sq);
  static uint64_t getCastlingHash(bool whiteKingSide, bool whiteQueenSide,
                                  bool blackKingSide, bool blackQueenSide);
  static uint64_t getEnPassantHash(Square epTarget);
  static uint64_t getToMoveHash(bool isWhite);
};
