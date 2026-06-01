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
