#include "utils/MagicBitboards.hpp"

/*
 * for an example of how they are currently being used in code see
 * bitboard attacks = magicbitboards::getqueenattacks(index, occupancy);
 * from queenmoves.cpp
 * it calls a function that given and index (which is the queen location)
 * and occupancy which is (friendly | enemy, notice how it is not neccesarily
 * "relevent") is returns all the possible attack squares the queen can engage.
 * the target piece is retrieved from the mailbox. the returned bitboard of
 * attacks should be as quick as indexing into an array, skipping the time taken
 * to slide a ray from each direction from the queens position for speed.
 */

// rayCast() now lives in the header as a constexpr function so the
// relevant-occupancy masks can be generated at compile time. Runtime table
// construction (magics, attack tables) will live here.

namespace MagicBitboards {

Bitboard generateRookAttacks(Square start, Bitboard blockers) {
  return rayCast(start, NORTH, blockers) | rayCast(start, SOUTH, blockers) |
         rayCast(start, EAST, blockers) | rayCast(start, WEST, blockers);
  // returned is a rayCast in each direction, stopped but including the blocker
  // it is simply what is generated, it is quite slow.
}

Bitboard generateBishopAttacks(Square start, Bitboard blockers) {
  return rayCast(start, NORTH_EAST, blockers) |
         rayCast(start, NORTH_WEST, blockers) |
         rayCast(start, SOUTH_EAST, blockers) |
         rayCast(start, SOUTH_WEST, blockers);
}

} // namespace MagicBitboards
