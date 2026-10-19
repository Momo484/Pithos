// Standalone tool: searches for rook/bishop magic multipliers and prints them
// as C++ array literals, ready to paste into MagicBitboards.hpp. It also prints
// the per-square shifts and re-verifies every magic against the naive slider, so
// a bad magic can never be silently baked in.
//
// Build:  cmake --build build --target generate_magics
// Run:    ./build/generate_magics

#include "utils/MagicBitboards.hpp"

#include <array>
#include <cstdint>
#include <cstdio>
#include <vector>

using namespace MagicBitboards;

namespace {

// Rebuilds the attack table for one square from a candidate magic and checks it
// agrees with the naive slider for every subset of the relevant mask.
bool verifyMagic(Square square, Bitboard mask, int shift, uint64_t magic,
                 Bitboard (*generateAttacks)(Square, Bitboard)) {
  const std::size_t n = std::size_t(1) << (64 - shift);
  std::vector<Bitboard> table(n, ~0ULL);

  Bitboard occ = 0ULL;
  do {
    unsigned idx = (occ * magic) >> shift;
    Bitboard attacks = generateAttacks(square, occ);
    if (table[idx] == ~0ULL) {
      table[idx] = attacks;
    } else if (table[idx] != attacks) {
      return false;
    }
    occ = (occ - mask) & mask;
  } while (occ != 0ULL);
  return true;
}

void printU64Array(const char *name, const std::array<uint64_t, 64> &values) {
  std::printf("constexpr std::array<Bitboard, Square::NUM_SQUARES> %s = {\n",
              name);
  for (int i = 0; i < 64; i++) {
    std::printf("    0x%016llxULL,%s", (unsigned long long)values[i],
                (i % 4 == 3) ? "\n" : " ");
  }
  std::printf("};\n\n");
}

void printIntArray(const char *name, const std::array<int, 64> &values) {
  std::printf("constexpr std::array<int, Square::NUM_SQUARES> %s = {", name);
  for (int i = 0; i < 64; i++) {
    if (i % 16 == 0) {
      std::printf("\n    ");
    }
    std::printf("%2d,", values[i]);
  }
  std::printf("\n};\n\n");
}

} // namespace

int main() {
  std::array<uint64_t, 64> rookMagics{};
  std::array<uint64_t, 64> bishopMagics{};

  for (int i = 0; i < Square::NUM_SQUARES; i++) {
    Square sq = Square::fromIndex(i);
    rookMagics[i] = findRookMagic(sq);
    bishopMagics[i] = findBishopMagic(sq);
  }

  // Cross-check every magic against the naive slider before printing it.
  bool allValid = true;
  for (int i = 0; i < Square::NUM_SQUARES; i++) {
    Square sq = Square::fromIndex(i);
    if (!verifyMagic(sq, rookRelevant[i], rookShifts[i], rookMagics[i],
                     generateRookAttacks)) {
      std::fprintf(stderr, "rook magic failed verification at square %d\n", i);
      allValid = false;
    }
    if (!verifyMagic(sq, bishopRelevant[i], bishopShifts[i], bishopMagics[i],
                     generateBishopAttacks)) {
      std::fprintf(stderr, "bishop magic failed verification at square %d\n",
                   i);
      allValid = false;
    }
  }

  std::printf("// cross-check: %s\n\n",
              allValid ? "all magics verified against the naive slider"
                       : "VERIFICATION FAILED - do not use these magics");

  printU64Array("ROOK_MAGICS", rookMagics);
  printU64Array("BISHOP_MAGICS", bishopMagics);
  printIntArray("ROOK_SHIFTS", rookShifts);
  printIntArray("BISHOP_SHIFTS", bishopShifts);

  return allValid ? 0 : 1;
}
