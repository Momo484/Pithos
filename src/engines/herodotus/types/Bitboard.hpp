#pragma once

#include <cstdint>

// Bitboard: 64 bits, one per square (bit index = rank * 8 + file).
// Rank 0 is rank 1 (bits 0-7: a1-h1) up to rank 7 (bits 56-63: a8-h8);
// within a rank, files run a (lowest bit) to h (highest bit).

typedef uint64_t Bitboard;

constexpr Bitboard EMPTY_BOARD = 0x0000000000000000ULL;
constexpr Bitboard FULL_BOARD = 0xFFFFFFFFFFFFFFFFULL;

// Rank masks: one row each.
constexpr Bitboard RANK_1 = 0x00000000000000FFULL;
constexpr Bitboard RANK_2 = 0x000000000000FF00ULL;
constexpr Bitboard RANK_3 = 0x0000000000FF0000ULL;
constexpr Bitboard RANK_4 = 0x00000000FF000000ULL;
constexpr Bitboard RANK_5 = 0x000000FF00000000ULL;
constexpr Bitboard RANK_6 = 0x0000FF0000000000ULL;
constexpr Bitboard RANK_7 = 0x00FF000000000000ULL;
constexpr Bitboard RANK_8 = 0xFF00000000000000ULL;

// File masks: one column each.
constexpr Bitboard FILE_A = 0x0101010101010101ULL;
constexpr Bitboard FILE_B = 0x0202020202020202ULL;
constexpr Bitboard FILE_C = 0x0404040404040404ULL;
constexpr Bitboard FILE_D = 0x0808080808080808ULL;
constexpr Bitboard FILE_E = 0x1010101010101010ULL;
constexpr Bitboard FILE_F = 0x2020202020202020ULL;
constexpr Bitboard FILE_G = 0x4040404040404040ULL;
constexpr Bitboard FILE_H = 0x8080808080808080ULL;
