#include "movegen/KnightMoves.hpp"
#include "types/Bitboard.hpp"
#include "utils/BitboardTables.hpp"

namespace MoveGen {

// Generates every pseudo-legal knight move for `side`. For each knight: look
// up its attack table, drop friendly squares, then emit the captures (with the
// piece from `mailbox`) and quiet moves as separate lists.
void generateKnightMoves(const PositionBitboards &pieces,
                         const Mailbox &mailbox, Bitboard friendly,
                         Bitboard enemy, Color side, std::vector<Move> &out) {
  Bitboard knightBB = pieces[side][Piece::KNIGHT];
  while (knightBB) {
    int index = __builtin_ctzll(knightBB);
    // Every square this knight can attack, straight from the precomputed table.
    Bitboard attacks = BitboardTables::knightAttacks[index];
    attacks &= ~friendly;
    Bitboard captures = attacks & enemy;
    Bitboard moves = attacks & ~enemy;
    Square from = Square::fromIndex(index);
    while (captures) {
      int captureIdx = __builtin_ctzll(captures);
      Square to = Square::fromIndex(captureIdx);
      out.push_back({side, Piece::KNIGHT, from, to, mailbox[captureIdx],
                     std::nullopt, false, false, false});

      captures &= captures - 1;
    }
    while (moves) {
      int moveIdx = __builtin_ctzll(moves);
      Square to = Square::fromIndex(moveIdx);
      out.push_back({side, Piece::KNIGHT, from, to, std::nullopt, std::nullopt, false, false, false});
      moves &= moves - 1;
    }

    knightBB &= knightBB - 1;
  }
}

} // namespace MoveGen
