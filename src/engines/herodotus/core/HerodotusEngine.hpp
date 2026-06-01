#pragma once

#include "../types/Types.hpp"
#include "../types/GameState.hpp"
#include "../types/Bitboard.hpp"
#include <optional>

/**
 * @file HerodotusEngine.hpp
 * @brief Core chess engine for the Herodotus chess system.
 *
 * The Herodotus Engine is a bitboard-based chess engine that maintains
 * chess positions using 64-bit bitboards for efficient move generation,
 * validation, and position analysis.
 *
 * Architecture:
 * - Uses separate bitboards for each piece type and color
 * - Maintains game state (castling rights, en passant, move counters)
 * - Provides position setup, printing, and analysis functionality
 *
 * @example
 * ```cpp
 * HerodotusEngine engine;
 * engine.initialise();           // Set up starting position
 * engine.printBoardState();      // Display the current position
 * ```
 */

/**
 * @class HerodotusEngine
 * @brief Main chess engine class for position representation and manipulation.
 *
 * The engine maintains:
 * - A 12-bitboard array: 6 piece types × 2 colors
 * - Complete game state (whose turn, castling rights, en passant)
 * - Utility functions for position analysis and display
 *
 * The internal bitboard layout uses little-endian encoding where bit 0 represents
 * the a1 square, bit 7 represents h1, bit 8 represents a2, etc.
 *
 * @invariant pieces[color][piece] contains exactly one bit for each piece of that type
 * @invariant gameState is kept in sync with piece positions
 */
class HerodotusEngine {
private:
  // ===== Data Members =====

  /**
   * @brief Bitboards for all pieces.
   *
   * Two-dimensional array of bitboards:
   * - First dimension: Color (0 = white, 1 = black)
   * - Second dimension: Piece type (0 = pawn, ..., 5 = king)
   *
   * Each bit in pieces[color][piece] represents a piece of that type and color.
   * A bit set to 1 means a piece occupies that square.
   */
  Bitboard pieces[2][6];

  /// Current game state (whose turn, castling rights, move counters, etc.)
  GameState gameState;

  // ===== Private Methods =====

  /**
   * @brief Sets up the default starting position for a chess game.
   *
   * Initializes all piece bitboards to represent the standard chess starting
   * position (after 0 moves):
   * - Pawns on rank 2 (white) and rank 7 (black)
   * - Other pieces arranged in their initial configuration
   *
   * The gameState is NOT modified by this method; call initialise() for full setup.
   *
   * @post All piece bitboards contain the starting position
   * @see initialise()
   */
  void setupStartingPosition();

  /**
   * @brief Clears all piece bitboards to represent an empty board.
   *
   * Sets all bitboards in the pieces array to 0, indicating no pieces
   * occupy any square.
   *
   * The gameState is NOT modified by this method.
   *
   * @post All pieces[i][j] == 0
   */
  void clearBoard();

  /**
   * @brief Generates a bitboard of all white pieces.
   *
   * Computes the union of all white piece bitboards using bitwise OR operations.
   * This is useful for checking occupancy and move generation.
   *
   * @return A bitboard with bits set for every square containing a white piece
   *
   * @example
   * ```cpp
   * Bitboard whitePieces = engine.getWhitePieces();
   * bool isWhiteOccupied = (whitePieces & (1ULL << squareIndex)) != 0;
   * ```
   */
  Bitboard getWhitePieces();

  /**
   * @brief Generates a bitboard of all black pieces.
   *
   * Computes the union of all black piece bitboards using bitwise OR operations.
   * This is useful for checking occupancy and move generation.
   *
   * @return A bitboard with bits set for every square containing a black piece
   *
   * @see getWhitePieces()
   */
  Bitboard getBlackPieces();

  /**
   * @brief Generates a bitboard of all occupied squares (both colors).
   *
   * Computes the union of all piece bitboards (both white and black).
   * This is useful for move generation and position analysis.
   *
   * @return A bitboard with bits set for every square containing any piece
   *
   * @example
   * ```cpp
   * Bitboard occupied = engine.getAllPieces();
   * Bitboard empty = ~occupied;  // Complement to get empty squares
   * ```
   */
  Bitboard getAllPieces();

public:
  /**
   * @brief Initializes the engine to the default starting chess position.
   *
   * This is the main initialization method that should be called once at engine
   * startup. It performs:
   * 1. Clears all bitboards to ensure a clean state
   * 2. Sets up the standard starting position
   * 3. Initializes game state with proper defaults (white to move, all castling
   *    rights available, no en passant)
   *
   * @post Engine is ready to play from the starting position
   *
   * @example
   * ```cpp
   * HerodotusEngine engine;
   * engine.initialise();           // Now ready to use
   * ```
   */
  void initialise();

  /**
   * @brief Prints the current board state to standard output.
   *
   * Displays the current position in a human-readable ASCII format:
   * - Uppercase letters for white pieces (P, N, B, R, Q, K)
   * - Lowercase letters for black pieces (p, n, b, r, q, k)
   * - Dots (.) for empty squares
   *
   * The board is printed with rank 8 at the top and rank 1 at the bottom,
   * showing the board from white's perspective.
   *
   * @example
   * ```
   * r n b q k b n r
   * p p p p p p p p
   * . . . . . . . .
   * . . . . . . . .
   * . . . . . . . .
   * . . . . . . . .
   * P P P P P P P P
   * R N B Q K B N R
   * ```
   */
  void printBoardState();
};
