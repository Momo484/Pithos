#pragma once

#include <cstdint>
#include <functional>
#include "Bitboard.hpp"

/**
 * @file Square.hpp
 * @brief Square representation and utilities for chess board positions.
 *
 * A square represents a single location on the 8x8 chess board identified by
 * rank (0-7, representing rows 1-8) and file (0-7, representing columns a-h).
 */

/**
 * @struct Square
 * @brief Represents a single square on the chess board.
 *
 * A square is uniquely identified by its rank (row) and file (column) coordinates.
 * Rank 0 corresponds to the first rank (rank 1 in chess notation), and rank 7
 * corresponds to the eighth rank. Similarly, file 0 corresponds to the a-file,
 * and file 7 corresponds to the h-file.
 *
 * @invariant rank must be in range [0, 7]
 * @invariant file must be in range [0, 7]
 *
 * @example
 * ```cpp
 * Square e4 = {3, 4};  // Rank 4 (index 3), file e (index 4)
 * Square a1 = {0, 0};  // Rank 1, file a
 * ```
 */
struct Square {
  /// Rank (row) of the square: 0-7 (representing ranks 1-8 in chess notation)
  std::uint8_t rank;

  /// File (column) of the square: 0-7 (representing files a-h in chess notation)
  std::uint8_t file;

  /**
   * @brief Validates that the square is within the board boundaries.
   *
   * @return true if both rank and file are in valid range [0, 7], false otherwise
   *
   * @example
   * ```cpp
   * Square sq = {3, 4};
   * if (!sq.isValid()) {
   *   // Handle invalid square
   * }
   * ```
   */
  bool isValid() const {
    return rank >= 0 && rank < 8 && file >= 0 && file < 8;
  }

  /**
   * @brief Equality comparison operator.
   *
   * Two squares are equal if both their rank and file coordinates match.
   *
   * @param other The square to compare against
   * @return true if squares have identical coordinates, false otherwise
   */
  bool operator==(const Square& other) const {
    return (this->rank == other.rank && this->file == other.file);
  }

  /**
   * @brief Inequality comparison operator.
   *
   * @param other The square to compare against
   * @return true if squares have different coordinates, false otherwise
   */
  bool operator!=(const Square& other) const { return !(*this == other); }

  /**
   * @brief Converts this square to a bitboard mask.
   *
   * Generates a 64-bit bitboard where only the bit corresponding to this
   * square's position is set. This is useful for combining multiple squares
   * or masking operations.
   *
   * The conversion formula: index = (rank << 3) | file
   *
   * @return A Bitboard with a single bit set at this square's position.
   *         Returns 0 if the square is invalid.
   *
   * @example
   * ```cpp
   * Square e1 = {0, 4};
   * Bitboard mask = e1.squareToU64();  // Single bit set at position 4
   * ```
   */
  Bitboard squareToU64() const {
    if (!isValid()) {
      return 0;
    }
    int index = (rank << 3) | file;
    return 1ULL << index;
  }

  /**
   * @brief Converts a linear bit index to a Square.
   *
   * Given a bit position from a bitboard (0-63), converts it back to
   * rank and file coordinates.
   *
   * @param index Bit position in the bitboard [0, 63]
   * @return Square representing the given bit position
   *
   * @note No bounds checking is performed on the index parameter.
   *
   * @example
   * ```cpp
   * Square sq = Square::fromIndex(4);  // Returns {0, 4} (e1)
   * ```
   */
  static Square fromIndex(int index) {
    return {static_cast<std::uint8_t>(index >> 3),
            static_cast<std::uint8_t>(index & 7)};
  }
};

/**
 * @struct SquareHash
 * @brief Hash function for Square objects to enable use in unordered containers.
 *
 * Implements a hash function combining rank and file using bit manipulation
 * to distribute values evenly across the hash space.
 *
 * @example
 * ```cpp
 * std::unordered_set<Square, SquareHash> squareSet;
 * squareSet.insert({3, 4});  // e4
 * ```
 */
struct SquareHash {
  /**
   * @brief Computes the hash value for a Square.
   *
   * Uses the rank and file values to compute a well-distributed hash code.
   * The hash combines two separate hash values using XOR and bit shifting.
   *
   * @param s The square to hash
   * @return Hash value for the square
   */
  std::size_t operator()(const Square& s) const {
    std::size_t h1 = std::hash<int>{}(s.rank);
    std::size_t h2 = std::hash<int>{}(s.file);
    return h1 ^ (h2 + 0x9e3778b9 + (h1 << 6) + (h1 >> 2));
  }
};
