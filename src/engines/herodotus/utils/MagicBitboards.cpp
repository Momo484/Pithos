#include "utils/MagicBitboards.hpp"
#include "types/Square.hpp"

namespace MagicBitboards {

Bitboard slideRay(Bitboard pos, int shift, Bitboard mask, bool north,
                  Bitboard blockers) {
  return 0ll;
}

Bitboard bishopAttacksFrom(int sq, Bitboard blockers) { return 0ll; }

Bitboard rookAttacksFrom(int sq, Bitboard blockers) { return 0ll; }

std::array<std::vector<Bitboard>, 64> bishopTable{};
std::array<std::vector<Bitboard>, 64> rookTable{};

void initMagicBitboards() {}

} // namespace MagicBitboards
