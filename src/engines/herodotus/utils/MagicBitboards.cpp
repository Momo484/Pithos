#include "utils/MagicBitboards.hpp"
#include "types/Square.hpp"
#include <cstdlib>

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

namespace MagicBitboards {

// this function simply casts a ray from the start square in the direction
// specified by the parameters filling the bitboard with ones in that direction
Bitboard rayCast(Square start, RayDirection dir) {
  Bitboard result = 0ULL;
  Bitboard curr = start.squareToU64();
  
  Bitboard wrapMask = 0ULL;
  if (dir == EAST || dir == NORTH_EAST || dir == SOUTH_EAST) {
    wrapMask = FILE_A;
  } else if (dir == WEST || dir == NORTH_WEST || dir == SOUTH_WEST) {
    wrapMask = FILE_H;
  }

  int absDir = std::abs(dir);
  while (curr != 0ULL) {
    result |= curr;
    curr = (dir > 0) ? (curr << absDir) : (curr >> absDir);
    curr &= ~wrapMask;
  }
  // removing the starting square
  result &= ~start.squareToU64();
  return result;
}
} // namespace MagicBitboards
