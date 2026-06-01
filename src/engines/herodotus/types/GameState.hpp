#pragma once

#include <cstdint>
#include <optional>
#include "Types.hpp"
#include "Square.hpp"

/**
 * @file GameState.hpp
 * @brief Game state and auxiliary information for chess position analysis.
 *
 * Contains all the auxiliary information required to represent a complete chess
 * game state beyond just piece positions. This includes whose turn it is, castling
 * rights, en passant targets, and move counters for the 50-move rule.
 */

/**
 * @struct GameState
 * @brief Encapsulates all auxiliary chess game state information.
 *
 * This structure maintains metadata about the current game position that cannot
 * be derived solely from piece positions. It tracks whose turn it is, what special
 * moves are available, and counters for move history.
 *
 * @invariant halfMoveClock is in range [0, 100+] (reset to 0 by captures/pawn moves)
 * @invariant fullMoveNumber is in range [1, ...)
 * @invariant castlingRights bits follow layout: [unused:2][blackQueenSide][blackKingSide][whiteQueenSide][whiteKingSide]:4
 */
struct GameState {
  /// Current active player (WHITE or BLACK)
  Color activeColor = Color::WHITE;

  /**
   * @brief Castling rights encoded as a bitmask.
   *
   * Each bit represents whether a specific castling right is available:
   * - Bit 0: White kingside castling (right)
   * - Bit 1: White queenside castling (left)
   * - Bit 2: Black kingside castling (right)
   * - Bit 3: Black queenside castling (left)
   * - Bits 4-7: Unused (should be 0)
   *
   * A bit value of 1 means the castling right is available.
   * The initial value 0x0F (0b00001111) means all four castling rights are available.
   *
   * Castling rights are lost when:
   * - A king moves
   * - A rook moves or is captured
   *
   * @example
   * ```cpp
   * // Check if white can castle kingside:
   * if (gameState.castlingRights & 0x1) { ... }
   *
   * // Remove white kingside castling:
   * gameState.castlingRights &= ~0x1;
   * ```
   */
  uint8_t castlingRights = 0b00001111;  // All castling rights available at start

  /**
   * @brief Halfmove clock for the 50-move rule.
   *
   * Counts the number of halfmoves (plies) since the last capture or pawn move.
   * The game is drawn if this reaches 100 (50 full moves without capture/pawn move).
   * This is reset to 0 whenever a capture occurs or a pawn moves.
   *
   * @invariant halfMoveClock >= 0
   */
  int halfMoveClock = 0;

  /**
   * @brief Full move number counter.
   *
   * Starts at 1 and increments after each of Black's moves. Used primarily
   * for notation and move history tracking.
   *
   * @invariant fullMoveNumber >= 1
   */
  int fullMoveNumber = 1;

  /**
   * @brief En passant target square (if available), nullopt otherwise.
   *
   * When a pawn advances two squares from its starting position, this holds
   * the coordinate of the square "behind" the pawn that an enemy pawn can
   * capture on (en passant rule).
   *
   * This is only meaningful during the halfmove immediately following a
   * double pawn push.
   *
   * @note The square represents the en passant capture square, not the
   *       destination square of the two-square pawn move.
   *
   * @example
   * ```cpp
   * // After white pawn moves from e2 to e4:
   * gameState.enPassant = Square{2, 4};  // e3 (the capture square)
   * ```
   */
  std::optional<Square> enPassant = std::nullopt;

  /**
   * @brief Position of the white king.
   *
   * Cached for quick access during move validation (checking, castling).
   * Must be kept in sync with the white king bitboard.
   */
  Square whiteKing = {0, 4};  // Default to e1

  /**
   * @brief Position of the black king.
   *
   * Cached for quick access during move validation (checking, castling).
   * Must be kept in sync with the black king bitboard.
   */
  Square blackKing = {7, 4};  // Default to e8
};
