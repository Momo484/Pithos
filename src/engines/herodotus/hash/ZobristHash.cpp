#include "ZobristHash.hpp"
#include <cctype>
#include <random>

/**
 * @file ZobristHash.cpp
 * @brief Implementation of Zobrist hash generation for chess positions.
 */

// Static member definitions
bool ZobristHash::initialised = false;
std::array<std::array<uint64_t, ZobristHash::NUM_PIECES>, ZobristHash::NUM_SQUARES> ZobristHash::pieceSquareTable;
uint64_t ZobristHash::whiteToMove;
std::array<uint64_t, 16> ZobristHash::castlingRights;
std::array<uint64_t, 8> ZobristHash::enPassantFiles;

int ZobristHash::getPieceIndex(char symbol) {
  switch (std::tolower(symbol)) {
  case 'p':
    return std::isupper(symbol) ? 0 : 6;
  case 'n':
    return std::isupper(symbol) ? 1 : 7;
  case 'b':
    return std::isupper(symbol) ? 2 : 8;
  case 'r':
    return std::isupper(symbol) ? 3 : 9;
  case 'q':
    return std::isupper(symbol) ? 4 : 10;
  case 'k':
    return std::isupper(symbol) ? 5 : 11;
  default:
    return -1;
  }
}

void ZobristHash::initialise() {
  if (initialised)
    return;

  // Use a fixed seed for reproducible, deterministic hash generation across runs
  std::mt19937_64 rng(0xDEADBEEF);
  std::uniform_int_distribution<uint64_t> dist;

  // Generate unique random numbers for each piece on each square
  // This creates NUM_SQUARES * NUM_PIECES (64 * 12 = 768) unique hash values
  for (auto& row : pieceSquareTable) {
    for (auto& val : row) {
      val = dist(rng);
    }
  }

  // Generate random values for move turn indicator
  whiteToMove = dist(rng);

  // Generate random values for all 16 castling rights configurations
  for (auto& val : castlingRights) {
    val = dist(rng);
  }

  // Generate random values for each en passant file
  for (auto& val : enPassantFiles) {
    val = dist(rng);
  }

  initialised = true;
}

uint64_t ZobristHash::getPieceHash(char symbol, Square sq) {
  int pieceIdx = ZobristHash::getPieceIndex(symbol);
  if (pieceIdx < 0)
    return 0;

  int squareIdx = sq.rank * 8 + sq.file;
  if (squareIdx < 0 || squareIdx >= NUM_SQUARES)
    return 0;

  return pieceSquareTable[squareIdx][pieceIdx];
}

uint64_t ZobristHash::getCastlingHash(bool whiteKingSide, bool whiteQueenSide,
                                      bool blackKingSide, bool blackQueenSide) {
  // Encode castling rights as a 4-bit index
  int idx = 0;
  if (whiteKingSide)
    idx |= 0x1;
  if (whiteQueenSide)
    idx |= 0x2;
  if (blackKingSide)
    idx |= 0x4;
  if (blackQueenSide)
    idx |= 0x8;

  return castlingRights[idx];
}

uint64_t ZobristHash::getEnPassantHash(Square epTarget) {
  if (epTarget.file < 0 || epTarget.file >= 8)
    return 0;

  return enPassantFiles[epTarget.file];
}

uint64_t ZobristHash::getToMoveHash(bool isWhite) {
  // Convention: return the hash when it's white's turn, 0 for black's turn
  // Toggling move is done by XORing twice (once to clear, once to set)
  return isWhite ? whiteToMove : 0;
}
