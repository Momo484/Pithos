#include "movegen/KingMoves.hpp"
#include "types/Bitboard.hpp"
#include "types/Square.hpp"
#include "utils/BitboardTables.hpp"

namespace MoveGen {
void generateKingMoves(const PositionBitboards &pieces, const Mailbox &mailbox,
                       Bitboard friendly, Bitboard enemy, Bitboard all,
                       Color side, const GameState &state,
                       std::vector<Move> &out) {

  bool isWhite = side == Color::WHITE;
  Bitboard kingBB = pieces[side][Piece::KING];
  int fromSq = __builtin_ctzll(kingBB);
  Square from = Square::fromIndex(fromSq);

  Bitboard kingMoves = BitboardTables::kingAttacks[fromSq];
  Bitboard friendlyShared = kingMoves & friendly;
  kingMoves ^= friendlyShared;

  Bitboard openMoves = kingMoves & ~enemy;
  Bitboard captureMoves = kingMoves & enemy;

  // Lets first deal with openMoves
  while (openMoves) {
    int toSq = __builtin_ctzll(openMoves);
    Square to = Square::fromIndex(toSq);
    Move openMove = {side,         Piece::KING, from,  to,   std::nullopt,
                     std::nullopt, false,       false, false};
    out.push_back(openMove);
    openMoves &= openMoves - 1;
  }

  // Now capture moves
  while (captureMoves) {
    int toSq = __builtin_ctzll(captureMoves);
    Square to = Square::fromIndex(toSq);
    Piece captured = mailbox[toSq];
    Move captureMove = {side,         Piece::KING, from,  to,   captured,
                        std::nullopt, false,       false, false};
    out.push_back(captureMove);
    captureMoves &= captureMoves - 1;
  }

  // Now we have to deal with castling. we have to check if the spaces between
  // king and rook is empty, for each respective side, and also if they have
  // castling rights. The engine will be responsible for legalising it, and
  // ensuring that the king doesnt move into or through check.
  // initially we are going to create it seperatly for white and black, but we
  // might be able to combine it later
  if (isWhite) {
    if (state.castlingRights & GameState::WHITE_KINGSIDE_CASTLE) {
      Bitboard between =
          BitboardTables::betweenSquares(RANK_1 & FILE_E, RANK_1 & FILE_H);
      if ((between & all) == 0) {
        out.push_back({side,
                       Piece::KING,
                       from,
                       {0, 6},
                       std::nullopt,
                       std::nullopt,
                       false,
                       true,
                       false});
      }
    }
    if (state.castlingRights & GameState::WHITE_QUEENSIDE_CASTLE) {
      Bitboard between =
          BitboardTables::betweenSquares(RANK_1 & FILE_E, RANK_1 & FILE_A);
      if ((between & all) == 0) {
        out.push_back({side,
                       Piece::KING,
                       from,
                       {0, 2},
                       std::nullopt,
                       std::nullopt,
                       false,
                       true,
                       false});
      }
    }
  } else {
    if (state.castlingRights & GameState::BLACK_KINGSIDE_CASTLE) {
      Bitboard between =
          BitboardTables::betweenSquares(RANK_8 & FILE_E, RANK_8 & FILE_H);
      if ((between & all) == 0) {
        out.push_back({side,
                       Piece::KING,
                       from,
                       {7, 6},
                       std::nullopt,
                       std::nullopt,
                       false,
                       true,
                       false});
      }
    }
    if (state.castlingRights & GameState::BLACK_QUEENSIDE_CASTLE) {
      Bitboard between =
          BitboardTables::betweenSquares(RANK_8 & FILE_E, RANK_8 & FILE_A);
      if ((between & all) == 0) {
        out.push_back({side,
                       Piece::KING,
                       from,
                       {7, 2},
                       std::nullopt,
                       std::nullopt,
                       false,
                       true,
                       false});
      }
    }
  }
}
} // namespace MoveGen
