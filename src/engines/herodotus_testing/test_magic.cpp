#include <catch2/catch_test_macros.hpp>
#include "../herodotus/types/Bitboard.hpp"
#include "../herodotus/utils/MagicBitboards.hpp"

#include <array>

namespace {

// Independent ray-walk reference (mirrors the semantics used to build the
// magic tables: slide along a ray until a blocker is hit).
Bitboard slideRayRef(Bitboard pos, int shift, Bitboard mask, bool north,
                     Bitboard blockers) {
  Bitboard attacks = 0;
  while (pos) {
    pos = (north ? pos << shift : pos >> shift) & ~mask;
    if (pos == 0)
      break;
    attacks |= pos;
    if (pos & blockers)
      break;
  }
  return attacks;
}

Bitboard bishopAttacksRef(int sq, Bitboard blockers) {
  Bitboard pos = 1ULL << sq;
  Bitboard attacks = 0;
  attacks |= slideRayRef(pos, 9, FILE_A, true, blockers);
  attacks |= slideRayRef(pos, 7, FILE_H, true, blockers);
  attacks |= slideRayRef(pos, 7, FILE_A, false, blockers);
  attacks |= slideRayRef(pos, 9, FILE_H, false, blockers);
  return attacks;
}

Bitboard rookAttacksRef(int sq, Bitboard blockers) {
  Bitboard pos = 1ULL << sq;
  Bitboard attacks = 0;
  attacks |= slideRayRef(pos, 1, FILE_A, true, blockers);
  attacks |= slideRayRef(pos, 1, FILE_H, false, blockers);
  attacks |= slideRayRef(pos, 8, 0, false, blockers);
  attacks |= slideRayRef(pos, 8, 0, true, blockers);
  return attacks;
}

Bitboard queenAttacksRef(int sq, Bitboard blockers) {
  return bishopAttacksRef(sq, blockers) | rookAttacksRef(sq, blockers);
}

const Bitboard START_POSITION = RANK_1 | RANK_2 | RANK_7 | RANK_8;
const Bitboard CENTER_DENSE = 0x00FF00FF00FF00FFULL; // checkerboard-ish
const Bitboard BACK_RANKS = RANK_1 | RANK_8;

} // namespace

TEST_CASE("MagicBitboards: attacks match ray-walk reference", "[magic]") {
  MagicBitboards::initMagicBitboards();

  const std::array<Bitboard, 4> occupancies = {
      EMPTY_BOARD, START_POSITION, CENTER_DENSE, BACK_RANKS};

  for (int sq = 0; sq < Square::NUM_SQUARES; sq++) {
    for (Bitboard occ : occupancies) {
      INFO("square=" << sq << " occupancy=0x" << std::hex << occ);
      REQUIRE(MagicBitboards::getBishopAttacks(sq, occ) ==
              bishopAttacksRef(sq, occ));
      REQUIRE(MagicBitboards::getRookAttacks(sq, occ) ==
              rookAttacksRef(sq, occ));
      REQUIRE(MagicBitboards::getQueenAttacks(sq, occ) ==
              queenAttacksRef(sq, occ));
    }
  }
}

TEST_CASE("MagicBitboards: relevant occupancy masks have canonical size",
          "[magic]") {
  // Canonical relevant-occupancy bit counts (rows rank 1..rank 8).
  // Source: chessprogramming.wiki (classical approach, magic bitboards).
  const std::array<std::array<int, 8>, 8> canonicalBishop = {{
      {{6, 5, 5, 5, 5, 5, 5, 6}},
      {{5, 5, 5, 5, 5, 5, 5, 5}},
      {{5, 5, 7, 7, 7, 7, 5, 5}},
      {{5, 5, 7, 9, 9, 7, 5, 5}},
      {{5, 5, 7, 9, 9, 7, 5, 5}},
      {{5, 5, 7, 7, 7, 7, 5, 5}},
      {{5, 5, 5, 5, 5, 5, 5, 5}},
      {{6, 5, 5, 5, 5, 5, 5, 6}},
  }};

  const std::array<std::array<int, 8>, 8> canonicalRook = {{
      {{12, 11, 11, 11, 11, 11, 11, 12}},
      {{11, 10, 10, 10, 10, 10, 10, 11}},
      {{11, 10, 10, 10, 10, 10, 10, 11}},
      {{11, 10, 10, 10, 10, 10, 10, 11}},
      {{11, 10, 10, 10, 10, 10, 10, 11}},
      {{11, 10, 10, 10, 10, 10, 10, 11}},
      {{11, 10, 10, 10, 10, 10, 10, 11}},
      {{12, 11, 11, 11, 11, 11, 11, 12}},
  }};

  for (int sq = 0; sq < Square::NUM_SQUARES; sq++) {
    int rank = sq / 8; // 0 = rank 1
    int file = sq % 8;

    INFO("square=" << sq);
    REQUIRE(std::__popcount(MagicBitboards::bishopRelevant[sq]) ==
            canonicalBishop[rank][file]);
    REQUIRE(std::__popcount(MagicBitboards::rookRelevant[sq]) ==
            canonicalRook[rank][file]);
  }
}
