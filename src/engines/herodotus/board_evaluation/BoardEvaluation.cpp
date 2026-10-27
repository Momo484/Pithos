#include "core/HerodotusEngine.hpp"

namespace BoardEvaluation {
double evaluateCurrentPosition(HerodotusEngine &engine) {
  double evaluation = 0.0;
  Bitboard whitePieces = engine.getWhitePieces();
  Bitboard blackPieces = engine.getBlackPieces();

  // To start simple we are going to count piece values.
  int kingDiff = (std::__popcount(engine.pieces[Color::WHITE][Piece::KING])) -
                 (std::__popcount(engine.pieces[Color::BLACK][Piece::KING]));
  int queenDiff = (std::__popcount(engine.pieces[Color::WHITE][Piece::QUEEN])) -
                  (std::__popcount(engine.pieces[Color::BLACK][Piece::QUEEN]));
  int rookDiff = (std::__popcount(engine.pieces[Color::WHITE][Piece::ROOK])) -
                 (std::__popcount(engine.pieces[Color::BLACK][Piece::ROOK]));
  int bishopDiff =
      (std::__popcount(engine.pieces[Color::WHITE][Piece::BISHOP])) -
      (std::__popcount(engine.pieces[Color::BLACK][Piece::BISHOP]));
  int knightDiff =
      (std::__popcount(engine.pieces[Color::WHITE][Piece::KNIGHT])) -
      (std::__popcount(engine.pieces[Color::BLACK][Piece::KNIGHT]));
  int pawnDiff = (std::__popcount(engine.pieces[Color::WHITE][Piece::PAWN])) -
                 (std::__popcount(engine.pieces[Color::BLACK][Piece::PAWN]));
  // adding material value to the evaluation.
  evaluation += (200 * kingDiff) + (9 * queenDiff) + (5 * rookDiff) +
                (3 * (bishopDiff + knightDiff)) + (1 * pawnDiff);
  // NOTE: # Legal moves <=> mobility, but it is hard to do right now, we do
  // slightly later

  // Pawn conditions
  int doubledPawnDiff = countDoubledPawns(engine, Color::WHITE) -
                        countDoubledPawns(engine, Color::BLACK);
  int isolatedPawnDiff = countIsolatedPawns(engine, Color::WHITE) -
                         countIsolatedPawns(engine, Color::BLACK);
  int blockedPawnDiff = countBlockedPawns(engine, Color::WHITE) -
                        countBlockedPawns(engine, Color::BLACK);

  evaluation -= 0.5 * (doubledPawnDiff + isolatedPawnDiff + blockedPawnDiff);

  // returns in the perspective of the active colour.
  return evaluation * (-1 * (engine.gameState.activeColor == Color::BLACK));
}

int countDoubledPawns(HerodotusEngine &engine, Color color) {
  Bitboard pawns = engine.pieces[color][Piece::PAWN];
  // For each FILE we count the number of pawns, if greater than 1, then
  // consider it doubled, this counts tripled, but we shall not count it as
  // worse just yet.
  int doubledPawns = 0;
  for (int i = 0; i < files.size(); i++) {
    doubledPawns += std::max(std::__popcount(pawns & files[i] - 1), 0);
  }
  return doubledPawns;
}

int countIsolatedPawns(HerodotusEngine &engine, Color color) {
  Bitboard pawns = engine.pieces[color][Piece::PAWN];
  int isolatedPawns = 0;
  for (int i = 0; i < files.size(); i++) {
    int pawnsOnFile = std::__popcount(files[i] & pawns);
    int west = i - 1;
    int east = i + 1;
    bool isolatedWest = false;
    if (west >= 0) {
      if (!std::__popcount(files[west] & pawns)) {
        isolatedWest = true;
      }
    }
    bool isolatedEast = false;
    if (east < 8) {
      if (!std::__popcount(files[east] & pawns)) {
        isolatedEast = true;
      }
    }
    isolatedPawns += (isolatedEast && isolatedWest) ? pawnsOnFile : 0;
  }
  return isolatedPawns;
}

int countBlockedPawns(HerodotusEngine &engine, Color color) {
  Bitboard pawnsAdvanced = (color == Color::WHITE)
                               ? engine.pieces[color][Piece::PAWN] << 8
                               : engine.pieces[color][Piece::PAWN] >> 8;
  return std::__popcount(pawnsAdvanced & engine.getAllPieces());
}
} // namespace BoardEvaluation
