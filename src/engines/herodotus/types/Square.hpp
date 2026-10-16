#pragma once

#include <cstdint>
#include <functional>
#include "Bitboard.hpp"

// A square on the 8x8 board: rank (row, 0-7 = ranks 1-8) and file (column,
// 0-7 = files a-h).
struct Square {
  std::uint8_t rank;
  std::uint8_t file;

  bool isValid() const {
    return rank >= 0 && rank < 8 && file >= 0 && file < 8;
  }

  bool operator==(const Square& other) const {
    return (this->rank == other.rank && this->file == other.file);
  }

  bool operator!=(const Square& other) const { return !(*this == other); }

  // Bitboard with only this square's bit set (0 if the square is invalid).
  Bitboard squareToU64() const {
    if (!isValid()) {
      return 0;
    }
    int index = (rank << 3) | file;
    return 1ULL << index;
  }

  // Square at a linear bit index [0, 63].
  static Square fromIndex(int index) {
    return {static_cast<std::uint8_t>(index >> 3),
            static_cast<std::uint8_t>(index & 7)};
  }

  // Square of the lowest set bit (undefined for an empty bitboard).
  static Square fromIndex(Bitboard bitboard) {
    return Square::fromIndex(__builtin_ctzll(bitboard));
  }

  const static int NUM_SQUARES = 64;
};

// Hash functor so Square can be used in unordered containers.
struct SquareHash {
  std::size_t operator()(const Square& s) const {
    std::size_t h1 = std::hash<int>{}(s.rank);
    std::size_t h2 = std::hash<int>{}(s.file);
    return h1 ^ (h2 + 0x9e3778b9 + (h1 << 6) + (h1 >> 2));
  }
};
