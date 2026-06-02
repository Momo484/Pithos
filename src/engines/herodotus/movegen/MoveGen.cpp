#include "movegen/MoveGen.hpp"
#include "core/HerodotusEngine.hpp"
#include "movegen/PawnMoves.hpp"
#include "movegen/KnightMoves.hpp"
#include "movegen/BishopMoves.hpp"
#include "movegen/RookMoves.hpp"
#include "movegen/QueenMoves.hpp"
#include "movegen/KingMoves.hpp"

namespace MoveGen {

void generatePseudoLegalMoves(HerodotusEngine &engine, std::vector<Move> &out) {
    out.reserve(300);

    Color side = engine.gameState.activeColor;
    Bitboard friendly = (side == WHITE) ? engine.getWhitePieces()
                                        : engine.getBlackPieces();
    Bitboard enemy = (side == WHITE) ? engine.getBlackPieces()
                                     : engine.getWhitePieces();
    Bitboard all = engine.getAllPieces();

    generatePawnMoves(engine.pieces, engine.mailbox, friendly, enemy, all, side,
                      engine.gameState, out);
    generateKnightMoves(engine.pieces[side][KNIGHT], friendly, side, out);
    generateBishopMoves(engine.pieces[side][BISHOP], friendly, all, side, out);
    generateRookMoves(engine.pieces[side][ROOK], friendly, all, side, out);
    generateQueenMoves(engine.pieces[side][QUEEN], friendly, all, side, out);
    generateKingMoves(engine.pieces[side][KING], friendly, all, side,
                      engine.gameState, out);
}

} // namespace MoveGen
