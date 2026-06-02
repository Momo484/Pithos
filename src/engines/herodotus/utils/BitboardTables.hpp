#pragma once

#include "types/Bitboard.hpp"
#include "types/Square.hpp"
#include <array>
#include <cstdlib>

namespace BitboardTables {

/// Generates a 64x64 lookup table where Between[sq1][sq2] has bits set
/// for all squares strictly between two aligned squares on a rank, file,
/// or diagonal. Returns 0 if sq1 and sq2 are not aligned.
constexpr std::array<std::array<Bitboard, Square::NUM_SQUARES>, Square::NUM_SQUARES> generateBetween() {
  std::array<std::array<Bitboard, Square::NUM_SQUARES>, Square::NUM_SQUARES> table{};
  for (int sq1 = 0; sq1 < Square::NUM_SQUARES; ++sq1) {
    int r1 = sq1 >> 3, f1 = sq1 & 7;
    for (int sq2 = 0; sq2 < Square::NUM_SQUARES; ++sq2) {
      int r2 = sq2 >> 3, f2 = sq2 & 7;
      int dr = r2 - r1, df = f2 - f1;

      Bitboard b = 0;
      if (dr == 0 && df != 0) {
        int step = df > 0 ? 1 : -1;
        for (int f = f1 + step; f != f2; f += step)
          b |= 1ULL << (r1 * 8 + f);
      } else if (df == 0 && dr != 0) {
        int step = dr > 0 ? 1 : -1;
        for (int r = r1 + step; r != r2; r += step)
          b |= 1ULL << (r * 8 + f1);
      } else {
        int adr = dr >= 0 ? dr : -dr;
        int adf = df >= 0 ? df : -df;
        if (adr == adf && adr != 0) {
          int rs = dr > 0 ? 1 : -1;
          int fs = df > 0 ? 1 : -1;
          for (int r = r1 + rs, f = f1 + fs; r != r2; r += rs, f += fs)
            b |= 1ULL << (r * 8 + f);
        }
      }
      table[sq1][sq2] = b;
    }
  }
  return table;
}

inline constexpr auto Between = generateBetween();

} // namespace BitboardTables
// Example usage.
//Bitboard betweenKingAndRook = BitboardTables::Between[fromSq][rookSq];
