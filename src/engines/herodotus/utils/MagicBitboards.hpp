#pragma once

#include "types/Bitboard.hpp"
#include "types/Square.hpp"
#include <array>
#include <vector>

namespace MagicBitboards {

// BishopAttacks
constexpr std::array<Bitboard, Square::NUM_SQUARES> generateBishopAttacks() {
  std::array<Bitboard, Square::NUM_SQUARES> table{};
  auto genDiagAttack = [](int shift, Bitboard mask, bool north,
                          Bitboard startPos) {
    Bitboard attacks = 0ULL;
    Bitboard currPos = startPos;
    while (currPos) {
      if (north) {
        currPos = (currPos << shift) & ~mask;
      } else {
        currPos = (currPos >> shift) & ~mask;
      }
      attacks |= currPos;
    }
    return attacks;
  };

  for (int i = 0; i < Square::NUM_SQUARES; i++) {
    Bitboard attacks = 0ULL;
    Bitboard startPos = 1ULL << i;
    // We are going to include the starting square in the mask and remove at the
    // end

    // north east
    attacks |= genDiagAttack(9, FILE_A, true, startPos);
    // north west
    attacks |= genDiagAttack(7, FILE_H, true, startPos);
    // south east
    attacks |= genDiagAttack(7, FILE_A, false, startPos);
    // south west
    attacks |= genDiagAttack(9, FILE_H, false, startPos);

    table[i] = attacks;
  }

  return table;
}

inline constexpr auto bishopAttacks = generateBishopAttacks();

constexpr std::array<Bitboard, Square::NUM_SQUARES> generateBishopRelevant() {
  std::array<Bitboard, Square::NUM_SQUARES> table{};
  Bitboard edges = FILE_A | FILE_H | RANK_1 | RANK_8;

  for (int i = 0; i < Square::NUM_SQUARES; i++) {
    table[i] = bishopAttacks[i] & ~edges;
  }

  return table;
}

inline constexpr auto bishopRelevant = generateBishopRelevant();

constexpr std::array<Bitboard, Square::NUM_SQUARES> generateRookAttacks() {
  std::array<Bitboard, Square::NUM_SQUARES> table{};
  auto genStraightAttack = [](int shift, Bitboard mask, bool left,
                              Bitboard startPos) {
    Bitboard attacks = 0ULL;
    Bitboard currPos = startPos;
    while (currPos) {
      if (left) {
        currPos = (currPos << shift) & ~mask;
      } else {
        currPos = (currPos >> shift) & ~mask;
      }
      attacks |= currPos;
    }
    return attacks;
  };

  for (int i = 0; i < Square::NUM_SQUARES; i++) {
    Bitboard attacks = 0ULL;
    Bitboard startPos = 1ULL << i;
    // We are going to include the starting square in the mask and remove at the
    // end

    // east
    attacks |= genStraightAttack(1, FILE_A, true, startPos);
    // west
    attacks |= genStraightAttack(1, FILE_H, false, startPos);
    // south
    attacks |= genStraightAttack(8, 0ULL, false, startPos);
    // north
    attacks |= genStraightAttack(8, 0ULL, true, startPos);

    table[i] = attacks;
  }

  return table;
}

inline constexpr auto rookAttacks = generateRookAttacks();

constexpr std::array<Bitboard, Square::NUM_SQUARES> generateRookRelevant() {
  std::array<Bitboard, Square::NUM_SQUARES> table{};
  Bitboard edges = FILE_A | FILE_H | RANK_1 | RANK_8;

  for (int i = 0; i < Square::NUM_SQUARES; i++) {
    table[i] = rookAttacks[i] & ~edges;
  }

  return table;
}

inline constexpr auto rookRelevant = generateRookRelevant();

// Magic numbers for perfect hashing
constexpr std::array<Bitboard, 64> bishopMagics = {{
    0x007FBFBFBFBFBFFFULL, 0x0000A060401007FCULL, 0x0001004008020002ULL,
    0x0000806004000000ULL, 0x0000100400000000ULL, 0x0000231804000000ULL,
    0x0000807802000000ULL, 0x0000204402000000ULL, 0x0000400802000001ULL,
    0x0000012040080002ULL, 0x0000804000080001ULL, 0x0000404040408002ULL,
    0x0000002020200400ULL, 0x0000008020001001ULL, 0x0000001008002482ULL,
    0x0000001FFE004100ULL, 0x00FFE0FFFFE0FFFFULL, 0x00007FFCFC00FFFFULL,
    0x0000401800204804ULL, 0x0000204020080008ULL, 0x0000200020080800ULL,
    0x0000004000800820ULL, 0x0000004040808000ULL, 0x0000000802002060ULL,
    0x008080180080FFE0ULL, 0x0000400400804008ULL, 0x0000200200804004ULL,
    0x0000200200204002ULL, 0x0000008004002001ULL, 0x0000800800801008ULL,
    0x0000400400401004ULL, 0x0000800040002002ULL, 0x0020202000808080ULL,
    0x0000201000808080ULL, 0x0001000800200040ULL, 0x0000002000200080ULL,
    0x0000004020008010ULL, 0x0000002020001008ULL, 0x0000000808001004ULL,
    0x0000001002002002ULL, 0x0040404200021008ULL, 0x0040404200021000ULL,
    0x0080808080820040ULL, 0x0000020040020010ULL, 0x0000020002002008ULL,
    0x0000000200080404ULL, 0x0000000801000801ULL, 0x0000000200020040ULL,
    0x0042424242408080ULL, 0x0081820242020080ULL, 0x0000010202020204ULL,
    0x0000020101010000ULL, 0x0000002001010100ULL, 0x0000000202020202ULL,
    0x0000000040404040ULL, 0x0000000010101010ULL, 0x0000000007070707ULL,
    0x0000000000020202ULL, 0x0000000000000000ULL, 0x0000000000000000ULL,
    0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL,
    0x0000000000000000ULL,
}};

constexpr std::array<Bitboard, 64> rookMagics = {{
    0x0080001020400080ULL, 0x0040001000200040ULL, 0x0080081000200080ULL,
    0x0080040800100080ULL, 0x0080020400080080ULL, 0x0080010200040080ULL,
    0x0080008001000200ULL, 0x0080002040800100ULL, 0x0000800040400080ULL,
    0x0000400020200040ULL, 0x0000800800100040ULL, 0x0000800400080040ULL,
    0x0000800200040040ULL, 0x0000800100020040ULL, 0x0000800040010040ULL,
    0x0000800040008040ULL, 0x0000208000400080ULL, 0x0000404000200040ULL,
    0x0000808000100040ULL, 0x0000808000080040ULL, 0x0000808000040040ULL,
    0x0000808000020040ULL, 0x0000808000010040ULL, 0x0000404000808040ULL,
    0x0000200040004080ULL, 0x0000400020004040ULL, 0x0000800010004040ULL,
    0x0000800008004040ULL, 0x0000800004004040ULL, 0x0000800002004040ULL,
    0x0000800001004040ULL, 0x0000400000808040ULL, 0x0000204000400080ULL,
    0x0000402000400040ULL, 0x0000801000400040ULL, 0x0000800800400040ULL,
    0x0000800400400040ULL, 0x0000800200400040ULL, 0x0000800100400040ULL,
    0x0000800040800040ULL, 0x0000200040808000ULL, 0x0000400020404000ULL,
    0x0000800010404000ULL, 0x0000800008404000ULL, 0x0000800004404000ULL,
    0x0000800002404000ULL, 0x0000800001404000ULL, 0x0000400000808000ULL,
    0x0000100040804000ULL, 0x0000200040802000ULL, 0x0000400040801000ULL,
    0x0000800040800800ULL, 0x0000800040800400ULL, 0x0000800040800200ULL,
    0x0000800040800100ULL, 0x0000800040800040ULL, 0x0000100040808000ULL,
    0x0000100040804000ULL, 0x0000200040804000ULL, 0x0000400040804000ULL,
    0x0000800040804000ULL, 0x0000800040804000ULL, 0x0000800040804000ULL,
    0x0000800040804000ULL,
}};

// Lookup tables — built at runtime by initMagicBitboards()
extern std::array<std::vector<Bitboard>, 64> bishopTable;
extern std::array<std::vector<Bitboard>, 64> rookTable;

/// Initialises the magic bitboard lookup tables. Must be called once at engine
/// startup before any move generation is performed.
void initMagicBitboards();

// ───── Inline lookup helpers ─────

inline Bitboard getBishopAttacks(int sq, Bitboard occupancy) {
  Bitboard occ = occupancy & bishopRelevant[sq];
  return bishopTable[sq][(occ * bishopMagics[sq]) >>
                         (64 - std::__popcount(bishopRelevant[sq]))];
}

inline Bitboard getRookAttacks(int sq, Bitboard occupancy) {
  Bitboard occ = occupancy & rookRelevant[sq];
  return rookTable[sq][(occ * rookMagics[sq]) >>
                       (64 - std::__popcount(rookRelevant[sq]))];
}

inline Bitboard getQueenAttacks(int sq, Bitboard occupancy) {
  return getBishopAttacks(sq, occupancy) | getRookAttacks(sq, occupancy);
}

} // namespace MagicBitboards
