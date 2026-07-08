#include "movegen/RookMoves.hpp"
#include "types/Types.hpp"
#include "utils/MagicBitboards.hpp"
#include <optional>

namespace MoveGen {

void generateRookMoves(const PositionBitboards &pieces, const Mailbox &mailbox,
                       Bitboard friendly, Bitboard enemy, Color side,
                       std::vector<Move> &out) {

  Bitboard rookBB = pieces[side][Piece::ROOK];
  Bitboard occupancy = friendly | enemy;
  while (rookBB) {
    int index = __builtin_ctzll(rookBB);
    Bitboard attacks = MagicBitboards::getRookAttacks(index, occupancy);
    Bitboard captures = attacks & enemy;
    Bitboard moves = attacks & ~enemy;
    Square from = Square::fromIndex(index);
    while (captures) {
      int captureIdx = __builtin_ctzll(captures);
      Square to = Square::fromIndex(captureIdx);
      out.push_back({side, Piece::ROOK, from, to, mailbox[captureIdx],
                     std::nullopt, false, false, false});
      captures &= captures - 1;
    }

    while (moves) {
      int moveIdx = __builtin_ctzll(moves);
      Square to = Square::fromIndex(moveIdx);
      out.push_back({side, Piece::ROOK, from, to, std::nullopt, std::nullopt,
                     false, false, false});
      moves &= moves - 1;
    }

    rookBB &= rookBB - 1;
  }
}

} // namespace MoveGen
