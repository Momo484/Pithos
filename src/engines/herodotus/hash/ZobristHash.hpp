#pragma once

#include "../types/Square.hpp"
#include "types/Types.hpp"
#include <array>
#include <cstdint>

// Zobrist hashing: a position's 64-bit hash is the XOR of random values for
// the piece placement (piece, square), the side to move, the castling rights
// and the en passant file. XOR is self-inverse (A ^ A = 0), so the hash can
// be updated incrementally as moves are made and undone.
// See https://en.wikipedia.org/wiki/Zobrist_hashing

// Static utility class: fill the tables once with initialise() at startup,
// then use the getters below. No instances are created.
class ZobristHash {
private:
  // Set once the tables have been filled.
  static bool initialised;

  // Number of distinct pieces (6 piece types x 2 colours).
  static constexpr int NUM_PIECES = 12;

  // Number of squares on the board.
  static constexpr int NUM_SQUARES = 64;

  // pieceSquareTable[square][piece] = random value for that piece on that
  // square. Piece indices 0-5 are white (P, N, B, R, Q, K), 6-11 black.
  static std::array<std::array<uint64_t, NUM_PIECES>, NUM_SQUARES> pieceSquareTable;

  // Value XORed in when it's white's turn to move; black's turn adds nothing.
  static uint64_t whiteToMove;

  // 16 values, one per castling rights configuration, indexed by the 4-bit
  // rights mask (bit 0 white kingside ... bit 3 black queenside).
  static std::array<uint64_t, 16> castlingRights;

  // 8 values, one per file (a-h), for the en passant target.
  static std::array<uint64_t, 8> enPassantFiles;

  // Piece symbol to table index: white 0-5 (uppercase), black 6-11 (lowercase),
  // -1 for an invalid symbol.
  static int getPieceIndex(char symbol);

public:
  // Fills every table with unique random values from a fixed seed (0xDEADBEEF)
  // so hashes are reproducible across runs. Call once at startup; later calls
  // return early.
  static void initialise();

  // Random value for `piece` of `pieceColor` on `sq`, or 0 if out of range.
  // XOR it into the hash to add the piece, XOR again to remove it.
  static uint64_t getPieceHash(Piece piece, Color pieceColor, Square sq);

  // Random value for this castling rights configuration; all 16 are stored,
  // indexed by the 4-bit mask built from the four bools.
  static uint64_t getCastlingHash(bool whiteKingSide, bool whiteQueenSide,
                                  bool blackKingSide, bool blackQueenSide);

  // Random value for `epTarget`'s file — only the file is used, 0 if out of
  // range.
  static uint64_t getEnPassantHash(Square epTarget);

  // White to move: the whiteToMove value. Black to move: 0, so the hash stays
  // unchanged. Toggle by XORing twice.
  static uint64_t getToMoveHash(Color color);
};
