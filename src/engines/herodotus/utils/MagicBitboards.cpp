#include "utils/MagicBitboards.hpp"
#include "types/Square.hpp"

namespace MagicBitboards {

Bitboard slideRay(Bitboard pos, int shift, Bitboard mask, bool north,
                  Bitboard blockers) {
  Bitboard attacks = 0;
  while (pos) {
    pos = (north ? pos << shift : pos >> shift) & ~mask;
    if (pos == 0)
      break;
    attacks |= pos;
    // we slide until a blocker appears
    if (pos & blockers)
      break;
  }
  return attacks;
}

Bitboard bishopAttacksFrom(int sq, Bitboard blockers) {
  Bitboard pos = 1ULL << sq;
  Bitboard attacks = 0;
  attacks |= slideRay(pos, 9, FILE_A, true, blockers);
  attacks |= slideRay(pos, 7, FILE_H, true, blockers);
  attacks |= slideRay(pos, 7, FILE_A, false, blockers);
  attacks |= slideRay(pos, 9, FILE_H, false, blockers);
  return attacks;
}

Bitboard rookAttacksFrom(int sq, Bitboard blockers) {
  Bitboard pos = 1ULL << sq;
  Bitboard attacks = 0;
  attacks |= slideRay(pos, 1, FILE_A, true, blockers);
  attacks |= slideRay(pos, 1, FILE_H, false, blockers);
  attacks |= slideRay(pos, 8, 0, false, blockers);
  attacks |= slideRay(pos, 8, 0, true, blockers);
  return attacks;
}

std::array<std::vector<Bitboard>, 64> bishopTable{};
std::array<std::vector<Bitboard>, 64> rookTable{};

void initMagicBitboards() {
  for (int sq = 0; sq < Square::NUM_SQUARES; sq++) {
    // ── Bishop ──
    Bitboard bMask = bishopRelevant[sq];
    int bBits = std::__popcount(bMask);
    bishopTable[sq].resize(1 << bBits);
    Bitboard subset = 0;
    do {
      int idx = (subset * bishopMagics[sq]) >> (64 - bBits);
      bishopTable[sq][idx] = bishopAttacksFrom(sq, subset);
      subset = (subset - bMask) & bMask;
    } while (subset);

    // ── Rook ──
    Bitboard rMask = rookRelevant[sq];
    int rBits = std::__popcount(rMask);
    rookTable[sq].resize(1 << rBits);
    subset = 0;
    do {
      int idx = (subset * rookMagics[sq]) >> (64 - rBits);
      rookTable[sq][idx] = rookAttacksFrom(sq, subset);
      subset = (subset - rMask) & rMask;
    } while (subset);
  }
}

} // namespace MagicBitboards
