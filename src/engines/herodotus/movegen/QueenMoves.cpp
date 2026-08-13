#include "movegen/QueenMoves.hpp"
#include "types/Types.hpp"
#include "utils/MagicBitboards.hpp"
#include <optional>

namespace MoveGen {

void generateQueenMoves(const PositionBitboards &pieces, const Mailbox &mailbox,
                        Bitboard friendly, Bitboard enemy, Color side,
                        std::vector<Move> &out) {

  Bitboard queenBB = pieces[side][Piece::QUEEN];
  Bitboard occupancy = friendly | enemy;
  while (queenBB) {
    int index = __builtin_ctzll(queenBB);
    Bitboard attacks = MagicBitboards::getQueenAttacks(index, occupancy);
    attacks &= ~friendly;
    Bitboard captures = attacks & enemy;
    Bitboard moves = attacks & ~enemy;
    Square from = Square::fromIndex(index);
    while (captures) {
      int captureIdx = __builtin_ctzll(captures);
      Square to = Square::fromIndex(captureIdx);
      out.push_back({side, Piece::QUEEN, from, to, mailbox[captureIdx],
                     std::nullopt, false, false, false});
      captures &= captures - 1;
    }

    while (moves) {
      int moveIdx = __builtin_ctzll(moves);
      Square to = Square::fromIndex(moveIdx);
      out.push_back({side, Piece::QUEEN, from, to, std::nullopt, std::nullopt,
                     false, false, false});
      moves &= moves - 1;
    }

    queenBB &= queenBB - 1;
  }
}

} // namespace MoveGen
