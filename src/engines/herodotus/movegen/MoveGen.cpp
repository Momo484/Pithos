#include "movegen/MoveGen.hpp"
#include "core/HerodotusEngine.hpp"
#include "movegen/BishopMoves.hpp"
#include "movegen/KingMoves.hpp"
#include "movegen/KnightMoves.hpp"
#include "movegen/PawnMoves.hpp"
#include "movegen/QueenMoves.hpp"
#include "movegen/RookMoves.hpp"
#include "types/Types.hpp"
#include "utils/BitboardTables.hpp"
#include "utils/MagicBitboards.hpp"
#include <cstdint>

namespace MoveGen {

// Generates every pseudo-legal move for the engine's current position by
// calling each piece generator in turn with the right bitboards.
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

// True if `side`'s king is attacked. Works backwards from the king: gather
// every attack that can reach its square, drop friendlies, then use the
// mailbox to confirm the piece on each threat square is the type that
// actually attacks along that line.
bool isKingChecked(HerodotusEngine &engine, Color side) {
  // This is somewhat long because I have not thought ahead and modularised
  // some of my move logic.

  Bitboard friendly =
      (side == WHITE) ? engine.getWhitePieces() : engine.getBlackPieces();
  Bitboard enemy =
      (side == WHITE) ? engine.getBlackPieces() : engine.getWhitePieces();
  Bitboard all = engine.getAllPieces();
  Bitboard kingBB = engine.pieces[side][Piece::KING];
  int kingIdx = __builtin_ctzll(kingBB);

  // Let's start with bishops/queens.
  // Diagonal threats: bishop attacks from the king square, minus friendlies.
  // We prune with the mailbox to ensure the threatening pieces are of the
  // right type.
  Bitboard diagThreats =
      MagicBitboards::getBishopAttacks(kingIdx, all) & ~friendly;
  while (diagThreats) {
    int threatIdx = __builtin_ctzll(diagThreats);
    if (engine.mailbox[threatIdx] == Piece::BISHOP ||
        engine.mailbox[threatIdx] == Piece::QUEEN) {
      return true;
    }
    diagThreats &= diagThreats - 1;
  }
  // Straight threats: same again along ranks and files with rook attacks.
  Bitboard straightThreats =
      MagicBitboards::getRookAttacks(kingIdx, all) & ~friendly;
  while (straightThreats) {
    int threatIdx = __builtin_ctzll(straightThreats);
    if (engine.mailbox[threatIdx] == Piece::ROOK ||
        engine.mailbox[threatIdx] == Piece::QUEEN) {
      return true;
    }
    straightThreats &= straightThreats - 1;
  }

  // Knight threats: the king's own attack table works here, since knight
  // attacks are symmetric.
  Bitboard knightThreats = BitboardTables::knightAttacks[kingIdx] & ~friendly;
  while (knightThreats) {
    int threatIdx = __builtin_ctzll(knightThreats);
    if (engine.mailbox[threatIdx] == Piece::KNIGHT) {
      return true;
    }

    knightThreats &= knightThreats - 1;
  }

  // Pawn threats: just the squares shifted 7 and 9, in front of me or behind
  // me depending on which colour I am.
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

// Filters the pseudo-legal list down to the moves that don't leave our own
// king in check, one validateMove call each.
std::vector<Move> pseudoToLegalMoves(HerodotusEngine &engine,
                                     const std::vector<Move> pseudoLegalMoves) {
  std::vector<Move> legalMoves;
  for (Move move : pseudoLegalMoves) {
    if (validateMove(engine, move)) {
      legalMoves.push_back(move);
    }
  }

  return legalMoves;
}

// True if `move` is legal: plays it out, checks the king, then undoes it.
// Castling additionally plays the transit square so the king can't move into
// or through check either.
bool validateMove(HerodotusEngine &engine, Move move) {

  if (move.isCastling) {
    // I think this is ok, since we would only validate a move made in the
    // right turn order, so checking the engine for which colour's turn it is
    // should result correctly.
    if (isKingChecked(engine, move.color)) {
      return false;
    }
    // determine if queenside or kingside
    int dir = 0;
    if (move.to.file > move.from.file) {
      // kingside castle (king moves to a higher file)
      dir = 1;
    } else {
      // queenside castle (king moves to a lower file)
      dir = -1;
    }
    Square kingSquare = Square::fromIndex(
        __builtin_ctzll(engine.pieces[move.color][Piece::KING]));
    Square transitSquare = {kingSquare.rank,
                            static_cast<uint8_t>(kingSquare.file + dir)};
    // The transit square should be clear as per move generation logic.
    Move transitMove = {move.color,   Piece::KING,  kingSquare, transitSquare,
                        std::nullopt, std::nullopt, false,      false};
    engine.makeMoveFast(transitMove);
    bool transitChecked = isKingChecked(engine, move.color);
    engine.undoMoveFast(transitMove);
    if (transitChecked) {
      return false;
    }
  }
  engine.makeMoveFast(move);
  bool legal = !isKingChecked(engine, move.color);
  engine.undoMoveFast(move);
  return legal;
}
} // namespace MoveGen
