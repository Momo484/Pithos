#include "movegen/KnightMoves.hpp"
#include "types/Bitboard.hpp"

namespace MoveGen {

void generateKnightMoves(const PositionBitboards &pieces,
                         const Mailbox &mailbox, Bitboard friendly,
                         Bitboard enemy, Color side, std::vector<Move> &out) {
  // Knight moves seem ok, i'll do it manually to start with bit shifts and
  // masks but later it would be a good idea to move to precomputed attack
  // tables probably.

  /**
   * A knight at any point has at most 8 moves, so we deal with 8 unique moves
   * that can all hopefully be deduced as bitshifts and masks For example if the
   * knight does a move two to the left and up one, it can't possible get to
   * files G and H without wrapping around, so we find the bitshift
   * corresponding to that move, then mask off those two files. I will determine
   * the shifts and files below, the move with be denoted with cardinal
   * direction as a basis but the first letter denotes the direction of the two
   * jump.
   *   - As usuall after we Xor with friendly peices to delete that move and do
   * it twice for enemy captures
   *   - I have just clocked that it might be neccessary to compute each move
   * type e.g NW separately, otherwise
   *   - We have no good indication of where the knight originated, this
   * hopefully is improved upon later with optimisation or attack tables or
   * something.
   */

  /**
   *
   * Shifting requiredments
   * ----------------------
   * NE (Up two, right once)
   * - To go up twice it is << 16, then to move to the right once it is << 1
   * - => << 17
   * - It is possible that a knight illegally wraps from the H file to the A
   * - We have some problems,
   * file so we must mask it with FILE_A so:
   * - => knightBB << 17 & ~FILE_A.
   *
   * NW
   * - =>  knightBB << 15 ~FILE_H
   *
   * EN (Right two, up once)
   * - to go up once is << 8, then to move to the right twice is << 2, so
   * - It is possible to wrap to FILE_A or FILE_B.
   * - => KnightBB << 10 & FILE_A & ~FILE_B
   *
   * WN
   * => knightBB << 6 & ~FILE_G & ~FILE_H
   *
   * SE
   * - To go down twice is >> 16, to go right once is << 1
   * - => knightBB >> 15 & ~FILE_A
   *
   * SW
   * - => knightBB >> 17 & ~FILE_H
   *
   * ES
   * - To go down once is >> 8, to go right twice is << 2
   * - => knightBB >> 6 & ~FILE_A & ~FILE_B
   *
   * WS
   * - => >> 10 & ~FILE_G & ~FILE_H.
   */

  Bitboard knightBB = pieces[side][Piece::KNIGHT];

  // We switch this whole logic to attack tables and stuff when i figure out
  // what they are and how it works.
  struct {
    int shift;
    Bitboard mask;
  } const dirs[8] = {
      {17, ~FILE_A},
      {15, ~FILE_H},
      {10, ~(FILE_A | FILE_B)},
      {6, ~(FILE_G | FILE_H)},
      {-15, ~FILE_A},
      {-17, ~FILE_H},
      {-6, ~(FILE_A | FILE_B)},
      {-10, ~(FILE_G | FILE_H)},
  };

  // using lambda functions for simplicity and to reduce repeated code.
  auto processDir = [&](int shift, Bitboard mask) {
    Bitboard targets =
        (shift > 0 ? (knightBB << shift) : (knightBB >> -shift)) & mask;
    targets &= ~friendly;

    auto addMoves = [&](Bitboard bits, bool isCapture) {
      while (bits) {
        int idx = __builtin_ctzll(bits);
        Square to = Square::fromIndex(idx);
        Square from = Square::fromIndex(idx - shift);
        out.push_back(
            {side, Piece::KNIGHT, from, to,
             isCapture ? std::optional<Piece>(mailbox[idx]) : std::nullopt,
             std::nullopt, false, false, false});
        bits &= bits - 1;
      }
    };

    addMoves(targets & ~enemy, false);
    addMoves(targets & enemy, true);
  };

  for (auto [shift, mask] : dirs) {
    processDir(shift, mask);
  }
}

} // namespace MoveGen
