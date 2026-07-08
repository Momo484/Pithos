#include "movegen/BishopMoves.hpp"
#include "types/Types.hpp"
#include "utils/MagicBitboards.hpp"
#include <optional>

namespace MoveGen {

void generateBishopMoves(const PositionBitboards &pieces,
                         const Mailbox &mailbox, Bitboard friendly,
                         Bitboard enemy, Color side, std::vector<Move> &out) {
  Bitboard bishopBB = pieces[side][Piece::BISHOP];
  Bitboard occupancy = friendly | enemy;
  while (bishopBB) {
    int index = __builtin_ctzll(bishopBB);
    Bitboard attacks = MagicBitboards::getBishopAttacks(index, occupancy);
    Bitboard captures = attacks & enemy;
    Bitboard moves = attacks & ~enemy;
    Square from = Square::fromIndex(index);
    while (captures) {
      int captureIdx = __builtin_ctzll(captures);
      Square to = Square::fromIndex(captureIdx);
      out.push_back({side, Piece::BISHOP, from, to, mailbox[captureIdx],
                     std::nullopt, false, false, false});
      captures &= captures - 1;
    }

    while (moves) {
      int moveIdx = __builtin_ctzll(moves);
      Square to = Square::fromIndex(moveIdx);
      out.push_back({side, Piece::BISHOP, from, to, std::nullopt, std::nullopt,
                     false, false, false});
      moves &= moves - 1;
    }

    bishopBB &= bishopBB - 1;
  }
}

} // namespace MoveGen
