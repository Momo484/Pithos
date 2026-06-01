#pragma once

#include "../types/Square.hpp"
#include <cstdint>

/**
 * @file ZobristHash.hpp
 * @brief Zobrist hashing for efficient position transposition tables.
 *
 * Zobrist hashing is a technique for generating a unique hash value for any
 * chess position. It's named after Albert Zobrist and is commonly used in chess
 * engines for transposition table lookups and move ordering.
 *
 * The hash is computed by XORing together random 64-bit numbers:
 * - A unique number for each (piece, square) combination
 * - A number for whose turn it is (white to move)
 * - Numbers for each possible castling rights configuration
 * - Numbers for each en passant file
 *
 * The mathematical property of XOR (self-inverse: A ⊕ A = 0) allows for
 * efficient incremental hash updates when pieces move.
 *
 * @see https://en.wikipedia.org/wiki/Zobrist_hashing
 */

/**
 * @class ZobristHash
 * @brief Static utility class for Zobrist hash generation and management.
 *
 * Provides methods to generate and retrieve Zobrist hash components for
 * different aspects of the game state (piece positions, castling, en passant, etc.).
 *
 * Usage is through static methods only; no instances of this class are created.
 *
 * @note The hash values are deterministically generated using a fixed seed
 *       (0xDEADBEEF) to ensure reproducible results across runs for debugging
 *       and testing purposes.
 *
 * @example
 * ```cpp
 * // Initialize the Zobrist hash tables (must be called once at startup)
 * ZobristHash::initialise();
 *
 * // Get hash for a white pawn on e4
 * uint64_t hash = ZobristHash::getPieceHash('P', Square{3, 4});
 *
 * // Get hash for castling rights (all four castling moves available)
 * uint64_t castlingHash = ZobristHash::getCastlingHash(true, true, true, true);
 *
 * // Get hash indicating it's white's turn to move
 * uint64_t turnHash = ZobristHash::getToMoveHash(true);
 * ```
 */
class ZobristHash {
private:
  /// Indicates whether the hash tables have been initialized
  static bool initialised;

  /// Number of distinct pieces (6 piece types × 2 colors)
  static constexpr int NUM_PIECES = 12;

  /// Number of squares on the board (8×8 chessboard)
  static constexpr int NUM_SQUARES = 64;

  /**
   * @brief Piece-square hash table.
   *
   * Stores a unique random 64-bit hash for each combination of:
   * - A specific square (0-63)
   * - A specific piece (0-11: white pawn, white knight, ..., black king)
   *
   * pieceSquareTable[square][piece] gives the hash for that piece on that square.
   *
   * Mapping of piece indices:
   * - 0-5: White pieces (Pawn, Knight, Bishop, Rook, Queen, King)
   * - 6-11: Black pieces (Pawn, Knight, Bishop, Rook, Queen, King)
   */
  static uint64_t pieceSquareTable[NUM_SQUARES][NUM_PIECES];

  /// Hash value indicating it's white's turn to move
  static uint64_t whiteToMove;

  /**
   * @brief Castling rights hash table.
   *
   * Stores a unique random 64-bit hash for each possible castling rights
   * configuration (16 total: 2^4, since there are 4 independent castling rights).
   *
   * Index bit layout:
   * - Bit 0: White kingside castling
   * - Bit 1: White queenside castling
   * - Bit 2: Black kingside castling
   * - Bit 3: Black queenside castling
   */
  static uint64_t castlingRights[16];

  /**
   * @brief En passant file hash table.
   *
   * Stores a unique random 64-bit hash for each of the 8 files (a-h).
   * Used when an en passant capture is possible; the file indicates
   * which file the en passant target square is on.
   *
   * Index 0-7 corresponds to files a-h.
   */
  static uint64_t enPassantFiles[8];

  /**
   * @brief Maps a piece character to its index in the piece table.
   *
   * Converts a piece symbol (lowercase or uppercase letter) to a numeric
   * index for lookup in the piece-square table.
   *
   * Index mapping:
   * - White pieces (uppercase): 0-5 (P, N, B, R, Q, K)
   * - Black pieces (lowercase): 6-11 (p, n, b, r, q, k)
   *
   * @param symbol Chess piece symbol ('P', 'p', 'N', 'n', etc.)
   * @return Index in [0, 11] for valid pieces, -1 for invalid symbols
   *
   * @example
   * ```cpp
   * ZobristHash::getPieceIndex('P');  // Returns 0 (white pawn)
   * ZobristHash::getPieceIndex('p');  // Returns 6 (black pawn)
   * ZobristHash::getPieceIndex('X');  // Returns -1 (invalid)
   * ```
   */
  static int getPieceIndex(char symbol);

public:
  /**
   * @brief Initializes all Zobrist hash tables with random values.
   *
   * Must be called once at engine startup before any hash functions are used.
   * Uses a fixed seed (0xDEADBEEF) for deterministic, reproducible results.
   *
   * This method is idempotent: calling it multiple times has no additional effect
   * after the first call (subsequent calls return early).
   *
   * @note This should be called during engine initialization, typically in main().
   *
   * @example
   * ```cpp
   * int main() {
   *   ZobristHash::initialise();  // Initialize once
   *   // ... rest of engine code ...
   * }
   * ```
   */
  static void initialise();

  /**
   * @brief Retrieves the hash for a piece on a specific square.
   *
   * Returns the precomputed random value associated with placing the given
   * piece on the given square. Used for incremental hash updates when pieces move.
   *
   * @param symbol Chess piece symbol (uppercase for white, lowercase for black)
   *               Valid symbols: P, N, B, R, Q, K (or lowercase variants)
   * @param sq The square where the piece is located
   *
   * @return The 64-bit hash value for the piece-square combination,
   *         or 0 if the piece symbol is invalid or the square is out of bounds
   *
   * @note To XOR a piece into a position hash: position_hash ^= getPieceHash(piece, square);
   * @note To XOR a piece out of a position hash: position_hash ^= getPieceHash(piece, square);
   *       (since A ⊕ A = 0 and ⊕ is commutative)
   *
   * @example
   * ```cpp
   * // Get hash for white queen on d4
   * uint64_t hash = ZobristHash::getPieceHash('Q', Square{3, 3});
   * ```
   */
  static uint64_t getPieceHash(char symbol, Square sq);

  /**
   * @brief Retrieves the hash for a given castling rights configuration.
   *
   * Returns the precomputed random value associated with the specified
   * castling rights availability. Each of the 16 possible combinations
   * (0-15) has a unique hash.
   *
   * @param whiteKingSide true if white can castle kingside (short)
   * @param whiteQueenSide true if white can castle queenside (long)
   * @param blackKingSide true if black can castle kingside (short)
   * @param blackQueenSide true if black can castle queenside (long)
   *
   * @return The 64-bit hash value for this castling rights configuration
   *
   * @example
   * ```cpp
   * // No castling available
   * uint64_t hash = ZobristHash::getCastlingHash(false, false, false, false);
   *
   * // All castling rights available
   * uint64_t hash = ZobristHash::getCastlingHash(true, true, true, true);
   * ```
   */
  static uint64_t getCastlingHash(bool whiteKingSide, bool whiteQueenSide,
                                  bool blackKingSide, bool blackQueenSide);

  /**
   * @brief Retrieves the hash for an en passant target file.
   *
   * Returns the precomputed random value associated with en passant being
   * possible on a specific file. The rank of the en passant square can be
   * inferred from context.
   *
   * @param epTarget The square that represents the en passant target
   *                 (typically one rank ahead of the pawn that moved two squares)
   *
   * @return The 64-bit hash value for this en passant opportunity,
   *         or 0 if the file is out of bounds
   *
   * @note Only the file of epTarget is used; the rank is ignored.
   *
   * @example
   * ```cpp
   * // En passant available on the e-file (e3 after white e2-e4)
   * uint64_t hash = ZobristHash::getEnPassantHash(Square{2, 4});
   * ```
   */
  static uint64_t getEnPassantHash(Square epTarget);

  /**
   * @brief Retrieves the hash for whose turn it is to move.
   *
   * Returns a hash value that should be XORed into the position hash to
   * indicate whose turn it is. For white to move, returns the precomputed
   * value; for black to move, returns 0 (which means the hash remains unchanged,
   * as is conventional in Zobrist hashing).
   *
   * @param isWhite true if it's white's turn to move, false if it's black's turn
   *
   * @return The hash value to XOR: a fixed value for white, 0 for black
   *
   * @example
   * ```cpp
   * uint64_t hash = position_hash;
   * if (should_toggle_move) {
   *   hash ^= ZobristHash::getToMoveHash(true);   // Toggle to white
   *   hash ^= ZobristHash::getToMoveHash(false);  // (both operations cancel if toggling)
   * }
   * ```
   */
  static uint64_t getToMoveHash(bool isWhite);
};
