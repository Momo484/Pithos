#include "movegen/MoveGen.hpp"
#include "core/HerodotusEngine.hpp"
#include "movegen/BishopMoves.hpp"
#include "movegen/KingMoves.hpp"
#include "movegen/KnightMoves.hpp"
#include "movegen/PawnMoves.hpp"
#include "movegen/QueenMoves.hpp"
#include "movegen/RookMoves.hpp"

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

} // namespace MoveGen
