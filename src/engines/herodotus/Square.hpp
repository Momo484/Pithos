#pragma once
#include <cstddef>
#include <functional>
#include <cstdint>

typedef uint64_t U64;

struct Square {
  std::uint8_t rank; // 0 to 7 (the row) (1 to 8)
  std::uint8_t file; // 0 to 7 (the column) (a to h);
 

  bool operator==(const Square &other) const {
    return (this->rank == other.rank && this->file == other.file);
  }

  bool operator!=(const Square &other) const { return !(*this == other); }

  /**
   * translate the square rank and file into a bitboard mask for the square.
   * @return U64 mask of square.
   */
  U64 squareToU64() const {
    int index = (rank << 3) | file;
    return 1ULL << index;
  }
};

struct SquareHash {
  std::size_t operator()(const Square &s) const {
    std::size_t h1 = std::hash<int>{}(s.rank);
    std::size_t h2 = std::hash<int>{}(s.file);

    return h1 ^ (h2 + 0x9e3778b9 + (h1 << 6) + (h1 >> 2));
  }
};
