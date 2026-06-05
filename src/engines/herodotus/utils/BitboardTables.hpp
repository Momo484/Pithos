#pragma once

#include "types/Bitboard.hpp"
#include "types/Square.hpp"
#include <array>
#include <cstdlib>

namespace BitboardTables {

/// Generates a 64x64 lookup table where Between[sq1][sq2] has bits set
/// for all squares strictly between two aligned squares on a rank, file,
/// or diagonal. Returns 0 if sq1 and sq2 are not aligned.
constexpr std::array<std::array<Bitboard, Square::NUM_SQUARES>,
                     Square::NUM_SQUARES>
generateBetween() {
  std::array<std::array<Bitboard, Square::NUM_SQUARES>, Square::NUM_SQUARES>
      table{};
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

/// Convenience wrapper: accepts bitboard masks (single-bit) and returns squares
/// strictly between them. Extracts the square index via ctzll internally.
inline Bitboard betweenSquares(Bitboard sq1, Bitboard sq2) {
  return Between[__builtin_ctzll(sq1)][__builtin_ctzll(sq2)];
}

// Precomputed knight attack table [square_index -> attacked squares bitmask].
constexpr std::array<Bitboard, Square::NUM_SQUARES> generateKnightAttacks() {
  std::array<Bitboard, Square::NUM_SQUARES> table{};
  for (int i = 0; i < Square::NUM_SQUARES; i++) {
    Bitboard curPos = 1ULL << i;
    Bitboard attacks =
        ((curPos << 17) & ~(FILE_A)) | ((curPos << 15) & ~(FILE_H)) |
        ((curPos << 10) & ~(FILE_A | FILE_B)) |
        ((curPos << 6) & ~(FILE_G | FILE_H)) | ((curPos >> 15) & ~(FILE_A)) |
        ((curPos >> 17) & ~(FILE_H)) | ((curPos >> 6) & ~(FILE_A | FILE_B)) |
        ((curPos >> 10) & ~(FILE_G | FILE_H));
    table[i] = attacks;
  }
  return table;
}

inline constexpr auto knightAttacks = generateKnightAttacks();

// Precomputed king attack table [square_index -> attacked squares bitmask].
// Generate all 8 pseudo-legal king moves.
// Each shift direction is masked to prevent horizontal wraparound.
// The file masks here filter the TARGET square after the shift:
//   - Shifts that increase file (+1) wrap from h-file to a-file,
//     so mask out a-file targets: & ~FILE_A
//   - Shifts that decrease file (-1) wrap from a-file to h-file,
//     so mask out h-file targets: & ~FILE_H
// Vertical-only shifts (<< 8 / >> 8) don't change file, so no
// wraparound — the bits simply shift off the board at rank 8/1.
//
// Direction   Shift   File change   Wrap from   Target mask
// ─────────────────────────────────────────────────────────
// east        << 1    +1            h-file      ~FILE_A
// west        >> 1    -1            a-file      ~FILE_H
// north-east  << 9    +1            h-file      ~FILE_A
// north-west  << 7    -1            a-file      ~FILE_H
// south-east  >> 7    +1            h-file      ~FILE_A
// south-west  >> 9    -1            a-file      ~FILE_H
// north       << 8     0            —           none
// south       >> 8     0            —           none

// Then, separate into open squares (empty) and captures (enemy),
// discarding any squares occupied by friendly pieces.
constexpr std::array<Bitboard, Square::NUM_SQUARES> generateKingAttacks() {
  std::array<Bitboard, Square::NUM_SQUARES> table{};
  for (int i = 0; i < Square::NUM_SQUARES; i++) {
    Bitboard kingBB = 1ULL << i;
    table[i] = ((kingBB << 1) & ~FILE_A) | ((kingBB >> 1) & ~FILE_H) |
               (kingBB << 8) | (kingBB >> 8) | ((kingBB << 7) & ~FILE_H) |
               ((kingBB << 9) & ~FILE_A) | ((kingBB >> 7) & ~FILE_A) |
               ((kingBB >> 9) & ~FILE_H);
  }
  return table;
}

inline constexpr auto kingAttacks = generateKingAttacks();

} // namespace BitboardTables
