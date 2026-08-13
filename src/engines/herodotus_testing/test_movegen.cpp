#include <catch2/catch_test_macros.hpp>
#include "../herodotus/core/HerodotusEngine.hpp"
#include "../herodotus/movegen/MoveGen.hpp"
#include "../herodotus/types/Move.hpp"

#include <vector>

namespace {

void setup(HerodotusEngine &e, const std::string &fen) {
  if (fen.empty()) {
    e.initialise();
  } else {
    e.loadFEN(fen);
  }
}

} // namespace

TEST_CASE("MoveGen: starting position has 20 legal moves with expected splits",
          "[movegen]") {
  HerodotusEngine engine;
  setup(engine, "");

  std::vector<Move> moves = engine.generateAllLegalMoves();
  REQUIRE(moves.size() == 20);

  int pawnMoves = 0, knightMoves = 0, bishopMoves = 0, rookMoves = 0,
      queenMoves = 0, kingMoves = 0;
  for (const Move &m : moves) {
    switch (m.piece) {
    case Piece::PAWN:
      pawnMoves++;
      break;
    case Piece::KNIGHT:
      knightMoves++;
      break;
    case Piece::BISHOP:
      bishopMoves++;
      break;
    case Piece::ROOK:
      rookMoves++;
      break;
    case Piece::QUEEN:
      queenMoves++;
      break;
    case Piece::KING:
      kingMoves++;
      break;
    default:
      break;
    }
    REQUIRE_FALSE(m.isCastling);
    REQUIRE_FALSE(m.isEnPassant);
  }

  REQUIRE(pawnMoves == 16);
  REQUIRE(knightMoves == 4);
  REQUIRE(bishopMoves == 0);
  REQUIRE(rookMoves == 0);
  REQUIRE(queenMoves == 0);
  REQUIRE(kingMoves == 0);
}

TEST_CASE("MoveGen: every legal move leaves the mover's king out of check",
          "[movegen]") {
  const std::vector<std::string> positions = {
      "",
      "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",
      "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",
      "4k3/8/8/3pP4/8/8/8/4K3 w - d6 0 1",
      "4k3/P7/8/8/8/8/8/4K3 w - - 0 1",
      "8/8/8/8/8/8/5k2/4K2R b K - 0 1",
  };

  for (const std::string &fen : positions) {
    INFO("position: " << (fen.empty() ? std::string("start") : fen));

    HerodotusEngine engine;
    setup(engine, fen);

    std::vector<Move> moves = engine.generateAllLegalMoves();
    REQUIRE_FALSE(moves.empty());

    for (const Move &m : moves) {
      engine.makeMove(m);

      // Both sides must have exactly one king afterwards.
      REQUIRE(std::__popcount(engine.getPieceBitboard(Color::WHITE, Piece::KING)) == 1);
      REQUIRE(std::__popcount(engine.getPieceBitboard(Color::BLACK, Piece::KING)) == 1);

      // The mover must not have left their own king in check.
      REQUIRE_FALSE(MoveGen::isKingChecked(engine, m.color));

      engine.undoMove();
    }
  }
}
