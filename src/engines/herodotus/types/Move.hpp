#pragma once

#include <optional>
#include "Types.hpp"
#include "Square.hpp"

/**
 * @file Move.hpp
 * @brief Move representation for the Herodotus chess engine.
 *
 * A move encapsulates all information about a single chess move,
 * including source and destination, piece type, captured pieces, and
 * special move information (promotion, en passant).
 */

/**
 * @struct Move
 * @brief Represents a single move in chess.
 *
 * A move is fully specified by:
 * - The piece being moved
 * - The source and destination squares
 * - Whether a piece was captured
 * - Whether the move promotes a pawn
 * - Whether the move is an en passant capture
 * - Whether the move is a pawn double push (making for a new en passant square)
 *
 * The color of the moving piece and piece type are explicitly stored
 * for quick access and validation without requiring board lookups.
 *
 * @example
 * ```cpp
 * // Moving white pawn from e2 to e4
 * Move move = {
 *   Color::WHITE,
 *   Piece::PAWN,
 *   Square{1, 4},      // e2
 *   Square{3, 4},      // e4
 *   std::nullopt,      // No capture
 *   std::nullopt,      // No promotion
 *   false              // Not en passant
 *   true               // triggers en passant square addition
 * };
 *
 * // White pawn promotes to queen on e8
 * Move promotion = {
 *   Color::WHITE,
 *   Piece::PAWN,
 *   Square{6, 4},      // e7
 *   Square{7, 4},      // e8
 *   std::nullopt,      // No capture
 *   Piece::QUEEN,      // Promote to queen
 *   false
 *   false
 * };
 *
 * // En passant capture
 * Move enPassant = {
 *   Color::WHITE,
 *   Piece::PAWN,
 *   Square{4, 4},      // e5
 *   Square{5, 3},      // d6 (where enemy pawn ends up)
 *   Piece::PAWN,       // Captured pawn
 *   std::nullopt,      // No promotion
 *   true               // This is en passant
 *   false
 * };
 * ```
 */
struct Move {
  /// Color of the piece being moved
  Color color;

  /// Type of piece being moved (PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING)
  Piece piece;

  /// Starting square of the move
  Square from;

  /// Destination square of the move
  Square to;

  /**
   * @brief The piece type that was captured, or nullopt if no capture.
   *
   * If this move captures an opponent's piece, this field contains the
   * type of the captured piece. For most positions, this will be the type
   * of the piece on the destination square, but for en passant moves, the
   * captured piece is not on the destination square.
   */
  std::optional<Piece> captured = std::nullopt;

  /**
   * @brief The piece type to promote to, or nullopt if not a promotion.
   *
   * When a pawn reaches the opposite end of the board (8th rank for white,
   * 1st rank for black), it must promote to a KNIGHT, BISHOP, ROOK, or QUEEN.
   * This field specifies which piece type the pawn becomes.
   *
   * @note A promotion is always a pawn move (piece == PAWN).
   * @note The promotion piece can never be a pawn or king.
   */
  std::optional<Piece> promotion = std::nullopt;

  /**
   * @brief Flag indicating whether this move is an en passant capture.
   *
   * En passant is a special pawn capture that occurs when an opponent's pawn
   * advances two squares from its starting position and lands beside another
   * pawn. The advancing pawn can be captured as if it had only moved one square.
   *
   * In an en passant move:
   * - The `to` square represents where the pawn moves (diagonally forward)
   * - The captured pawn is NOT on the `to` square (it's one rank back)
   * - The `captured` field will contain `Piece::PAWN`
   *
   * @note This flag helps differentiate en passant from other diagonal pawn moves.
   */
  bool isEnPassant = false;

  /**
   * @brief Flag indicating whether this move is a castling move.
   *
   * Castling is a special move where the king moves two squares toward a rook,
   * and the rook moves to the square the king crossed over.
   *
   * There are two types of castling:
   * - Kingside (short): King to g-file, rook to f-file
   * - Queenside (long): King to c-file, rook to d-file
   *
   * In a castling move:
   * - `piece` is always KING
   * - `from` is the starting king position (e1 for white, e8 for black)
   * - `to` is the destination (g1/g8 for kingside, c1/c8 for queenside)
   * - `captured` is always nullopt (castling only occurs when no pieces in the way)
   *
   * @note The rook movement is implicit in the castling move and must be
   *       handled separately when applying the move to the board.
   */
  bool isCastling = false;

  /**
   * @brief Flag indicating whether this move triggers an en passant square recording
   * Allowing the moving pawn to be en passanted. 
   *
   * @note The engine should record the en passant square addition, it is not within the move
   */
  bool isPawnDoublePush = false;
};
