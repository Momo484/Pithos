#pragma once

#include <cstdint>

/**
 * @file Bitboard.hpp
 * @brief Bitboard type definitions and utilities for efficient position representation.
 *
 * A bitboard is a 64-bit unsigned integer where each bit represents a square on the 
 * chess board. This allows for efficient square occupation queries and position manipulation
 * using bitwise operations.
 *
 * Bit layout (little-endian):
 * - Bits 0-7: Rank 1 (a1-h1)
 * - Bits 8-15: Rank 2 (a2-h2)
 * - ...
 * - Bits 56-63: Rank 8 (a8-h8)
 *
 * Within each rank, files are ordered a-h (bits 0, 1, 2, ..., 7).
 */

/// 64-bit bitboard type for position representation
typedef uint64_t Bitboard;

/// Constant representing an empty bitboard (no pieces on the board)
constexpr Bitboard EMPTY_BOARD = 0x0000000000000000ULL;

/// Constant representing a completely filled bitboard (all squares occupied)
constexpr Bitboard FULL_BOARD = 0xFFFFFFFFFFFFFFFFULL;

/// Rank masks — each covers one row (8 squares)
constexpr Bitboard RANK_1 = 0x00000000000000FFULL;
constexpr Bitboard RANK_2 = 0x000000000000FF00ULL;
constexpr Bitboard RANK_3 = 0x0000000000FF0000ULL;
constexpr Bitboard RANK_4 = 0x00000000FF000000ULL;
constexpr Bitboard RANK_5 = 0x000000FF00000000ULL;
constexpr Bitboard RANK_6 = 0x0000FF0000000000ULL;
constexpr Bitboard RANK_7 = 0x00FF000000000000ULL;
constexpr Bitboard RANK_8 = 0xFF00000000000000ULL;

/// File masks — each covers one column (8 squares)
constexpr Bitboard FILE_A = 0x0101010101010101ULL;
constexpr Bitboard FILE_B = 0x0202020202020202ULL;
constexpr Bitboard FILE_C = 0x0404040404040404ULL;
constexpr Bitboard FILE_D = 0x0808080808080808ULL;
constexpr Bitboard FILE_E = 0x1010101010101010ULL;
constexpr Bitboard FILE_F = 0x2020202020202020ULL;
constexpr Bitboard FILE_G = 0x4040404040404040ULL;
constexpr Bitboard FILE_H = 0x8080808080808080ULL;
