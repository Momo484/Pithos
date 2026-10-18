#include <catch2/catch_test_macros.hpp>
#include "../herodotus/types/Bitboard.hpp"
#include "../herodotus/utils/MagicBitboards.hpp"

#include <array>
#include <initializer_list>

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

// Build a bitboard from a list of square indices (rank * 8 + file).
Bitboard squares(std::initializer_list<int> idx) {
  Bitboard bb = 0ULL;
  for (int i : idx)
    bb |= 1ULL << i;
  return bb;
}

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

TEST_CASE("MagicBitboards: rayCast stops at board edges", "[magic][raycast]") {
  using namespace MagicBitboards;

  // ── a1 (corner: only north, east and the NE diagonal survive) ──
  const Square a1{0, 0};
  REQUIRE(rayCast(a1, NORTH) == squares({8, 16, 24, 32, 40, 48, 56}));   // a2..a8
  REQUIRE(rayCast(a1, EAST) == squares({1, 2, 3, 4, 5, 6, 7}));          // b1..h1
  REQUIRE(rayCast(a1, NORTH_EAST) ==
          squares({9, 18, 27, 36, 45, 54, 63}));                        // b2..h8
  REQUIRE(rayCast(a1, WEST) == EMPTY_BOARD);
  REQUIRE(rayCast(a1, SOUTH) == EMPTY_BOARD);
  REQUIRE(rayCast(a1, NORTH_WEST) == EMPTY_BOARD);                      // would wrap to h1
  REQUIRE(rayCast(a1, SOUTH_EAST) == EMPTY_BOARD);
  REQUIRE(rayCast(a1, SOUTH_WEST) == EMPTY_BOARD);

  // ── h1 ──
  const Square h1{0, 7};
  REQUIRE(rayCast(h1, WEST) == squares({6, 5, 4, 3, 2, 1, 0}));          // g1..a1
  REQUIRE(rayCast(h1, NORTH) ==
          squares({15, 23, 31, 39, 47, 55, 63}));                       // h2..h8
  REQUIRE(rayCast(h1, NORTH_WEST) ==
          squares({14, 21, 28, 35, 42, 49, 56}));                       // g2..a8
  REQUIRE(rayCast(h1, EAST) == EMPTY_BOARD);                            // would wrap to a2
  REQUIRE(rayCast(h1, NORTH_EAST) == EMPTY_BOARD);                      // would wrap to a3
  REQUIRE(rayCast(h1, SOUTH) == EMPTY_BOARD);
  REQUIRE(rayCast(h1, SOUTH_EAST) == EMPTY_BOARD);
  REQUIRE(rayCast(h1, SOUTH_WEST) == EMPTY_BOARD);

  // ── a8 ──
  const Square a8{7, 0};
  REQUIRE(rayCast(a8, SOUTH) == squares({48, 40, 32, 24, 16, 8, 0}));    // a7..a1
  REQUIRE(rayCast(a8, EAST) ==
          squares({57, 58, 59, 60, 61, 62, 63}));                       // b8..h8
  REQUIRE(rayCast(a8, SOUTH_EAST) ==
          squares({49, 42, 35, 28, 21, 14, 7}));                        // b7..h1
  REQUIRE(rayCast(a8, NORTH) == EMPTY_BOARD);
  REQUIRE(rayCast(a8, WEST) == EMPTY_BOARD);                            // would wrap to h7
  REQUIRE(rayCast(a8, NORTH_WEST) == EMPTY_BOARD);                      // would wrap to h8
  REQUIRE(rayCast(a8, NORTH_EAST) == EMPTY_BOARD);

  // ── h8 ──
  const Square h8{7, 7};
  REQUIRE(rayCast(h8, SOUTH) == squares({55, 47, 39, 31, 23, 15, 7}));    // h7..h1
  REQUIRE(rayCast(h8, WEST) ==
          squares({62, 61, 60, 59, 58, 57, 56}));                       // g8..a8
  REQUIRE(rayCast(h8, SOUTH_WEST) ==
          squares({54, 45, 36, 27, 18, 9, 0}));                         // g7..a1
  REQUIRE(rayCast(h8, NORTH) == EMPTY_BOARD);
  REQUIRE(rayCast(h8, EAST) == EMPTY_BOARD);
  REQUIRE(rayCast(h8, NORTH_WEST) == EMPTY_BOARD);
  REQUIRE(rayCast(h8, NORTH_EAST) == EMPTY_BOARD);
  REQUIRE(rayCast(h8, SOUTH_EAST) == EMPTY_BOARD);

  // ── e4 (interior square: all eight directions are full-length rays) ──
  const Square e4{3, 4};
  REQUIRE(rayCast(e4, NORTH) == squares({36, 44, 52, 60}));             // e5..e8
  REQUIRE(rayCast(e4, SOUTH) == squares({20, 12, 4}));                  // e3..e1
  REQUIRE(rayCast(e4, EAST) == squares({29, 30, 31}));                  // f4..h4
  REQUIRE(rayCast(e4, WEST) == squares({27, 26, 25, 24}));              // d4..a4
  REQUIRE(rayCast(e4, NORTH_EAST) == squares({37, 46, 55}));            // f5,g6,h7
  REQUIRE(rayCast(e4, NORTH_WEST) == squares({35, 42, 49, 56}));        // d5,c6,b7,a8
  REQUIRE(rayCast(e4, SOUTH_EAST) == squares({21, 14, 7}));             // f3,g2,h1
  REQUIRE(rayCast(e4, SOUTH_WEST) == squares({19, 10, 1}));             // d3,c2,b1
}
