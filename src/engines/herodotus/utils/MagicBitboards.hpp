#pragma once

#include "types/Bitboard.hpp"
#include "types/Square.hpp"
#include <array>
#include <vector>

namespace MagicBitboards {

constexpr std::array<Bitboard, Square::NUM_SQUARES> generateBishopRelevant() {
  std::array<Bitboard, Square::NUM_SQUARES> table{};
  return table;
}

inline constexpr auto bishopRelevant = generateBishopRelevant();

constexpr std::array<Bitboard, Square::NUM_SQUARES> generateRookRelevant() {
  std::array<Bitboard, Square::NUM_SQUARES> table{};
  return table;
}

inline constexpr auto rookRelevant = generateRookRelevant();

// Magic numbers for perfect hashing
constexpr std::array<Bitboard, 64> bishopMagics{};

constexpr std::array<Bitboard, 64> rookMagics = {};

// Lookup tables — built at runtime by initMagicBitboards()
extern std::array<std::vector<Bitboard>, 64> bishopTable;
extern std::array<std::vector<Bitboard>, 64> rookTable;

/// Initialises the magic bitboard lookup tables. Must be called once at engine
/// startup before any move generation is performed.
void initMagicBitboards();

// ───── Inline lookup helpers ─────
// https://rhysre.net/fast-chess-move-generation-with-magic-bitboards.html

inline Bitboard getBishopAttacks(int sq, Bitboard occupancy) { return 0ll; }

inline Bitboard getRookAttacks(int sq, Bitboard occupancy) { return 0ll; }

inline Bitboard getQueenAttacks(int sq, Bitboard occupancy) {
  return getBishopAttacks(sq, occupancy) | getRookAttacks(sq, occupancy);
}

} // namespace MagicBitboards
