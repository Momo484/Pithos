#pragma once
#include <cstdint>
#include <optional>
#include "Square.hpp"

enum Piece { PAWN, BISHOP, ROOK, KNIGHT, QUEEN, KING };

enum Color { WHITE, BLACK };

struct GameState {
  Color activeColor = Color::WHITE;
  // CastlingRights represented by uint8_T
  // first three, white QueenSide rook, white King, white KingSideRook...
  uint8_t castlingRights = 0b11111100;
  int halfMoveClock = 0;
  int fullMoveNumber = 0;
  // this meight not be most efficient, but i am going to use a square class for now
  std::optional<Square> enPassant = std::nullopt;
  // We also need to store the moveLastPlayed
  Square blackKing;
  Square whiteKing;

};

// Lets macro BitBoard.
typedef uint64_t U64;

class HerodotusEngine {
private:
  // Data ----------------------------------------------------

  // All of our peice position bit boards
  U64 pieces[2][6];
  GameState gameState;

  // Private Methods -----------------------------------------

  // Sets up the default starting values for all the bitboards
  // for a default chess starting position.
  /**
   * Sets all the pieces bitboards to the default chess starting postion
   */
  void setupStartingPosition();

  // Clears all the bitboards to 0ULL.
  /**
   * Clears all the Bitboards in peices to 0ULL
   */
  void clearBoard();

  /**
   * Generates a bitboard corresponding to the position of white pieces 
   */
  U64 getWhitePieces();

  /**
   * Generates a bitboard corresponding to the position of black pieces
   */
  U64 getBlackPieces();

  /**
   * Generates a bitboard corresponding to all occupied squares 
   */
  U64 getAllPieces();


public:
  /**
   * Initialises the enging, clears board, sets up starting position
   */
  void initialise();

  /**
   * prints the current state of the board.
   */
  void printBoardState();
};
