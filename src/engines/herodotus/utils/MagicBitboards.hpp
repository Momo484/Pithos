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

// this is a prototype for the general ray cast function that will be
// implemented in the cpp file.
Bitboard rayCast(Square start, RayDirection dir);

// This for each square, generates the relevent blocker squares for a rook.
constexpr std::array<std::array<Bitboard, Square::NUM_SQUARES>,
                     Square::NUM_SQUARES>
generateRookRelevant() {
  std::array<std::array<Bitboard, Square::NUM_SQUARES>, Square::NUM_SQUARES>
      rookRelevent = {};

  return rookRelevent;
}

// This is the actual array used throughout the code, as a result of the
// generator above.
constexpr auto rookRelevent = generateRookRelevant();
} // namespace MagicBitboards
