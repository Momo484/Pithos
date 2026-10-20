#include "utils/MagicBitboards.hpp"
#include "types/Square.hpp"
#include <cstdint>
#include <limits>
#include <random>
#include <vector>

// Magic bitboard support. Holds the naive slider generation (the reference used
// when building tables) and the search for the magic multipliers that compress
// a square's relevant occupancy into a small table index.

namespace MagicBitboards {

namespace {
std::array<std::array<Bitboard, ROOK_TABLE_SIZE>, Square::NUM_SQUARES>
    rookAttacks{};
std::array<std::array<Bitboard, BISHOP_TABLE_SIZE>, Square::NUM_SQUARES>
    bishopAttacks{};
} // namespace

Bitboard generateRookAttacks(Square start, Bitboard blockers) {
  // Slides each ray to the edge, stopping on (and including) a blocker.
  return rayCast(start, NORTH, blockers) | rayCast(start, SOUTH, blockers) |
         rayCast(start, EAST, blockers) | rayCast(start, WEST, blockers);
}

Bitboard generateBishopAttacks(Square start, Bitboard blockers) {
  // Slides each diagonal ray to the edge, stopping on (and including) a
  // blocker.
  return rayCast(start, NORTH_EAST, blockers) |
         rayCast(start, NORTH_WEST, blockers) |
         rayCast(start, SOUTH_EAST, blockers) |
         rayCast(start, SOUTH_WEST, blockers);
}

namespace {

// Finds a magic multiplier for `square` (relevant occupancy `mask`).
// `generateAttacks` is the naive slider, used both to learn the true attack set
// for every subset of `mask` and to test candidate magics.
//
// A candidate is rejected the moment two subsets with *different* attack sets
// land on the same index. Magics where colliding subsets share an attack set
// are fine (that is normal and expected, and is why a perfect hash is
// possible).
uint64_t findMagic(Square square, Bitboard mask,
                   Bitboard (*generateAttacks)(Square, Bitboard),
                   std::mt19937_64 &gen) {
  const int bits = std::__popcount(mask);
  const int shift = 64 - bits;
  const std::size_t n = std::size_t(1) << bits;

  // Every subset of the relevant mask (carry-rippler) and its true attacks.
  std::vector<Bitboard> subsets;
  std::vector<Bitboard> attacks;
  subsets.reserve(n);
  attacks.reserve(n);
  Bitboard occ = 0ULL;
  do {
    subsets.push_back(occ);
    attacks.push_back(generateAttacks(square, occ));
    occ = (occ - mask) & mask;
  } while (occ != 0ULL);

  std::uniform_int_distribution<uint64_t> dis(
      std::numeric_limits<uint64_t>::min(),
      std::numeric_limits<uint64_t>::max());

  std::vector<Bitboard> table(n);
  // epoch[idx] records which candidate last wrote table[idx]. Comparing it to
  // the current stamp says whether that slot is set for *this* candidate, so
  // the table never has to be cleared between candidates.
  std::vector<uint32_t> epoch(n, 0);
  uint32_t stamp = 0;

  while (true) {
    ++stamp;
    uint64_t magicCandidate = dis(gen) & dis(gen) & dis(gen);
    // Sparse candidates with enough high bits are far likelier to succeed.
    if (std::__popcount((mask * magicCandidate) & 0xFF00000000000000) < 6) {
      continue;
    }
    bool validCandidate = true;
    for (std::size_t i = 0; i < n; i++) {
      unsigned idx = (subsets[i] * magicCandidate) >> shift;
      if (epoch[idx] != stamp) {
        epoch[idx] = stamp;
        table[idx] = attacks[i];
      } else if (table[idx] != attacks[i]) {
        validCandidate = false;
        break;
      }
    }
    if (validCandidate) {
      return magicCandidate;
    }
  }
}

} // namespace

uint64_t findRookMagic(Square square) {
  // Seeded per square so each magic is reproducible and independent of call
  // order.
  std::mt19937_64 gen(0x9E3779B97F4A7C15ULL + square.squareToIndex());
  return findMagic(square, rookRelevant[square.squareToIndex()],
                   generateRookAttacks, gen);
}

uint64_t findBishopMagic(Square square) {
  std::mt19937_64 gen(0xC2B2AE3D27D4EB4FULL + square.squareToIndex());
  return findMagic(square, bishopRelevant[square.squareToIndex()],
                   generateBishopAttacks, gen);
}

// Builds both attack tables: for every square, walk every subset of the
// relevant mask (carry-rippler) and store the naive attack at the index the
// magic number maps it to.
void initMagicBitboards() {
  // Fill the rook attack table.
  for (int sq = 0; sq < 64; sq++) {
    uint64_t magic = ROOK_MAGICS[sq];
    Bitboard relevant = rookRelevant[sq];
    int shift = rookShifts[sq];
    Bitboard occ = 0ULL;
    do {
      int idx = (occ * magic) >> shift;
      rookAttacks[sq][idx] = generateRookAttacks(Square::fromIndex(sq), occ);
      occ = (occ - relevant) & relevant;
    } while (occ != 0);
  }
  // Fill the bishop attack table.
  for (int sq = 0; sq < 64; sq++) {
    uint64_t magic = BISHOP_MAGICS[sq];
    Bitboard relevant = bishopRelevant[sq];
    int shift = bishopShifts[sq];
    Bitboard occ = 0ULL;
    do {
      int idx = (occ * magic) >> shift;
      bishopAttacks[sq][idx] =
          generateBishopAttacks(Square::fromIndex(sq), occ);
      occ = (occ - relevant) & relevant;
    } while (occ != 0);
  }
}

// Index straight into the rook table with ((occupancy & relevant) * magic)
// >> shift.
Bitboard getRookAttacks(int index, Bitboard occupancy) {
  return rookAttacks[index]
                    [((occupancy & rookRelevant[index]) * ROOK_MAGICS[index]) >>
                     rookShifts[index]];
}

// Same again for the bishop table.
Bitboard getBishopAttacks(int index, Bitboard occupancy) {
  return bishopAttacks[index][((occupancy & bishopRelevant[index]) *
                               BISHOP_MAGICS[index]) >>
                              bishopShifts[index]];
}

// Rook and bishop attacks OR'd together — a queen's moves are their union.
Bitboard getQueenAttacks(int index, Bitboard occupancy) {
  return getRookAttacks(index, occupancy) | getBishopAttacks(index, occupancy);
}
} // namespace MagicBitboards
