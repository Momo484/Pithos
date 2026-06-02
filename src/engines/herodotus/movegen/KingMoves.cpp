#include "movegen/KingMoves.hpp"
#include "types/Bitboard.hpp"

namespace MoveGen {
void generateKingMoves(const PositionBitboards &pieces,
                       const Mailbox &mailbox, Bitboard friendly, Bitboard enemy,
                       Bitboard all, Color side, const GameState &state,
                       std::vector<Move> &out) {

  bool isWhite = side == Color::WHITE;
  Bitboard kingBB = pieces[side][Piece::KING];
  int fromSq = __builtin_ctzll(kingBB);
  Square from = Square::fromIndex(fromSq);

  // Generate all 8 pseudo-legal king moves.
  // Each shift direction is masked to prevent horizontal wraparound.
  // The file masks here filter the TARGET square after the shift:
  //   - Shifts that increase file (+1) wrap from h-file to a-file,
  //     so mask out a-file targets: & ~FILE_A
  //   - Shifts that decrease file (-1) wrap from a-file to h-file,
  //     so mask out h-file targets: & ~FILE_H
  // Vertical-only shifts (<< 8 / >> 8) don't change file, so no
  // wraparound — the bits simply shift off the board at rank 8/1.
  //
  // Direction   Shift   File change   Wrap from   Target mask
  // ─────────────────────────────────────────────────────────
  // east        << 1    +1            h-file      ~FILE_A
  // west        >> 1    -1            a-file      ~FILE_H
  // north-east  << 9    +1            h-file      ~FILE_A
  // north-west  << 7    -1            a-file      ~FILE_H
  // south-east  >> 7    +1            h-file      ~FILE_A
  // south-west  >> 9    -1            a-file      ~FILE_H
  // north       << 8     0            —           none
  // south       >> 8     0            —           none

  // Then, separate into open squares (empty) and captures (enemy),
  // discarding any squares occupied by friendly pieces.

  Bitboard kingMoves = ((kingBB << 1) & ~FILE_A) | ((kingBB >> 1) & ~FILE_H) |
                       (kingBB << 8) | (kingBB >> 8) |
                       ((kingBB << 7) & ~FILE_H) | ((kingBB << 9) & ~FILE_A) |
                       ((kingBB >> 7) & ~FILE_A) | ((kingBB >> 9) & ~FILE_H);

  Bitboard friendlyShared = kingMoves & friendly;
  // we validiate kingMoves now
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
  // I think i now need those bitboard with the space between two squares.
  // I can find a square index with FILE_X & RANK_Y, which is nice tho.

  
}
} // namespace MoveGen
