#include "movegen/MoveGen.hpp"
#include "core/HerodotusEngine.hpp"
#include "movegen/BishopMoves.hpp"
#include "movegen/KingMoves.hpp"
#include "movegen/KnightMoves.hpp"
#include "movegen/PawnMoves.hpp"
#include "movegen/QueenMoves.hpp"
#include "movegen/RookMoves.hpp"
#include "utils/MagicBitboards.hpp"
#include "utils/BitboardTables.hpp"

namespace MoveGen {

void generatePseudoLegalMoves(HerodotusEngine &engine, std::vector<Move> &out) {
  out.reserve(300);

  Color side = engine.gameState.activeColor;
  Bitboard friendly =
      (side == WHITE) ? engine.getWhitePieces() : engine.getBlackPieces();
  Bitboard enemy =
      (side == WHITE) ? engine.getBlackPieces() : engine.getWhitePieces();
  Bitboard all = engine.getAllPieces();

  generatePawnMoves(engine.pieces, engine.mailbox, friendly, enemy, all, side,
                    engine.gameState, out);
  generateKnightMoves(engine.pieces, engine.mailbox, friendly, enemy, side,
                      out);

  generateBishopMoves(engine.pieces, engine.mailbox, friendly, enemy, side,
                      out);

  generateRookMoves(engine.pieces, engine.mailbox, friendly, enemy, side, out);

  generateQueenMoves(engine.pieces, engine.mailbox, friendly, enemy, side, out);
  generateKingMoves(engine.pieces, engine.mailbox, friendly, enemy, all, side,
                    engine.gameState, out);
}

bool isKingChecked(HerodotusEngine &engine) {
  // This will be somwhat long because i have not thought ahead and improved modularised
  // some of my move logic.
  
  Color side = engine.gameState.activeColor;
  Bitboard friendly =
      (side == WHITE) ? engine.getWhitePieces() : engine.getBlackPieces();
  Bitboard enemy =
      (side == WHITE) ? engine.getBlackPieces() : engine.getWhitePieces();
  Bitboard all = engine.getAllPieces();
  Square kingSquare = (side == WHITE) ? engine.gameState.whiteKing : engine.gameState.blackKing;
  Bitboard kingBB = kingSquare.squareToU64();
  int kingIdx = __builtin_ctzll(kingBB);

  // Lets start with bishops/Queens
  // we get diagonal threats by finding bishopattacks from king position, then removing friendlies
  // we will have to prune using mailbox further, to ensure the atttacking pieces are of the right 
  // type.
  Bitboard diagThreats = MagicBitboards::getBishopAttacks(kingIdx, all) & ~friendly;
  while(diagThreats) {
    int threatIdx = __builtin_ctzll(diagThreats);
    if (engine.mailbox[threatIdx] == Piece::BISHOP || engine.mailbox[threatIdx] == Piece::QUEEN) {
      return true;
    }
    diagThreats &= diagThreats - 1;
  }
  // straight threats
  Bitboard straightThreats = MagicBitboards::getRookAttacks(kingIdx, all) & ~friendly;
  while (straightThreats) {
    int threatIdx = __builtin_ctzll(diagThreats);
    if (engine.mailbox[threatIdx] == Piece::ROOK || engine.mailbox[threatIdx] == Piece::QUEEN) {
      return true;
    }
    straightThreats &= straightThreats - 1;
  }

  //Knight Threats
  Bitboard knightThreats = BitboardTables::knightAttacks[kingIdx] & ~friendly;
  while(knightThreats) {
    int threatIdx = __builtin_ctzll(knightThreats);
    if (engine.mailbox[threatIdx] == Piece::KNIGHT) {
      return true;
    }

    knightThreats &= knightThreats - 1;
  }

  //Pawn Threats
  // i just have to check the fields bitShifted 7 and 9, in front of me, or behind me
  // depending on what colour i am
  //
  Bitboard pawnThreats = 0ULL;
  if (side == Color::WHITE) {
    pawnThreats |= ((kingBB << 7) & ~FILE_H);
    pawnThreats |= ((kingBB << 9) & ~FILE_A);
  } else {
    pawnThreats |= ((kingBB >> 7) & ~FILE_A);
    pawnThreats |= ((kingBB >> 9) & ~FILE_H);
  }
  pawnThreats = pawnThreats & ~friendly;
  while (pawnThreats) {
    int threatIdx = __builtin_ctzll(pawnThreats);
    if (engine.mailbox[threatIdx] == Piece::PAWN) {
      return true;
    }
    pawnThreats &= pawnThreats - 1;
  }

  return false;
}

std::vector<Move> pseudoToLegalMoves(HerodotusEngine &engine, const std::vector<Move> pseudoLegalMoves) {
  std::vector<Move> legalMoves;

  return legalMoves;
}

} // namespace MoveGen
