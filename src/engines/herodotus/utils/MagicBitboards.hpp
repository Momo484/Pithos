#pragma once

#include "types/Bitboard.hpp"
#include "types/Square.hpp"
#include <array>
#include <cstddef>
#include <vector>

/*
 * for an example of how they are currently being used in code see
 *
 * bitboard attacks = magicbitboards::getqueenattacks(index, occupancy);
 * from queenMoves.cpp
 *
 * it calls a function that given and index (which is the queen location)
 * and occupancy which is (friendly | enemy, notice how it is not neccesarily
 * "relevent"), We filter out attacks on friendly later in the process seen here
 *
 * Bitboard captures = attacks & enemy;
 * Bitboard moves = attacks & ~enemy;
 * from queenMoves.cpp
 *
 * is returns all the possible attack squares the queen can engage. the target
 * piece is retrieved from the mailbox. the returned bitboard of attacks should
 * be as quick as indexing into an array, skipping the time taken to slide a ray
 * from each direction from the queens position for speed.
 */

namespace MagicBitboards {

// left shift is positive, right shift is negative
enum RayDirection {
  NORTH = 8,
  SOUTH = -8,
  EAST = 1,
  WEST = -1,
  NORTH_EAST = 9,
  NORTH_WEST = 7,
  SOUTH_EAST = -7,
  SOUTH_WEST = -9
};

// this function simply casts a ray from the start square in the direction
// specified by the parameters filling the bitboard with ones in that direction
constexpr Bitboard rayCast(Square start, RayDirection dir) {
  Bitboard result = 0ULL;
  Bitboard curr = start.squareToU64();
  
  Bitboard wrapMask = 0ULL;
  if (dir == EAST || dir == NORTH_EAST || dir == SOUTH_EAST) {
    wrapMask = FILE_A;
  } else if (dir == WEST || dir == NORTH_WEST || dir == SOUTH_WEST) {
    wrapMask = FILE_H;
  }

  int d = static_cast<int>(dir);
  int absDir = d < 0 ? -d : d;
  while (curr != 0ULL) {
    result |= curr;
    curr = (d > 0) ? (curr << absDir) : (curr >> absDir);
    curr &= ~wrapMask;
  }
  // removing the starting square
  result &= ~start.squareToU64();
  return result;
}

// For each square, the relevant blocker squares for a rook: the four rays
// radiating from the square, with the terminal (board-edge) square of each ray
// removed. A blocker on a terminal square cannot change the attack set, so it
// is excluded; this is what lets the occupancy patterns be compressed.
constexpr std::array<Bitboard, Square::NUM_SQUARES> generateRookRelevant() {
  std::array<Bitboard, Square::NUM_SQUARES> rookRelevant{};

  for (int i = 0; i < Square::NUM_SQUARES; i++) {
    Square start = Square::fromIndex(i);

    Bitboard rays = rayCast(start, NORTH) | rayCast(start, SOUTH) |
                    rayCast(start, EAST) | rayCast(start, WEST);

    // The board-edge square each ray ends on (excluded from the mask).
    Bitboard endpoints = Square{7, start.file}.squareToU64() |
                         Square{0, start.file}.squareToU64() |
                         Square{start.rank, 7}.squareToU64() |
                         Square{start.rank, 0}.squareToU64();

    rookRelevant[i] = rays & ~endpoints;
  }

  return rookRelevant;
}

// This is the actual array used throughout the code, as a result of the
// generator above.
constexpr auto rookRelevant = generateRookRelevant();
} // namespace MagicBitboards
