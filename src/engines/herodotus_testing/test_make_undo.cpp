#include <catch2/catch_test_macros.hpp>
#include "../herodotus/core/HerodotusEngine.hpp"
#include "../herodotus/types/Move.hpp"

#include <string>
#include <vector>

namespace {

struct StateSnapshot {
  Bitboard pieces[Color::NUM_COLORS][Piece::NUM_PIECES];
  Mailbox mailbox;
  GameState gameState;
  uint64_t zobristHash = 0;
  size_t historySize = 0;
};

StateSnapshot snapshot(const HerodotusEngine &e) {
  StateSnapshot s;
  for (int c = 0; c < Color::NUM_COLORS; c++) {
    for (int p = 0; p < Piece::NUM_PIECES; p++) {
      s.pieces[c][p] =
          e.getPieceBitboard(static_cast<Color>(c), static_cast<Piece>(p));
    }
  }
  s.mailbox = e.getMailbox();
  s.gameState = e.getGameState();
  s.zobristHash = e.getZobristHash();
  s.historySize = e.getHistorySize();
  return s;
}

// Returns a description of the first differing field, or "" if identical.
std::string firstMismatch(const StateSnapshot &a, const StateSnapshot &b) {
  for (int c = 0; c < Color::NUM_COLORS; c++) {
    for (int p = 0; p < Piece::NUM_PIECES; p++) {
      if (a.pieces[c][p] != b.pieces[c][p]) {
        return "bitboard color=" + std::to_string(c) +
               " piece=" + std::to_string(p);
      }
    }
  }
  if (a.mailbox != b.mailbox)
    return "mailbox";
  if (a.gameState.activeColor != b.gameState.activeColor)
    return "activeColor";
  if (a.gameState.castlingRights != b.gameState.castlingRights)
    return "castlingRights";
  if (a.gameState.halfMoveClock != b.gameState.halfMoveClock)
    return "halfMoveClock";
  if (a.gameState.fullMoveNumber != b.gameState.fullMoveNumber)
    return "fullMoveNumber";
  if (a.gameState.enPassant != b.gameState.enPassant)
    return "enPassant";
  if (a.zobristHash != b.zobristHash)
    return "zobristHash";
  if (a.historySize != b.historySize)
    return "historySize";
  return "";
}

bool movesEqual(const std::vector<Move> &a, const std::vector<Move> &b) {
  if (a.size() != b.size())
    return false;
  for (size_t i = 0; i < a.size(); i++) {
    const Move &x = a[i];
    const Move &y = b[i];
    if (x.color != y.color || x.piece != y.piece || x.from != y.from ||
        x.to != y.to || x.captured != y.captured ||
        x.promotion != y.promotion || x.isEnPassant != y.isEnPassant ||
        x.isCastling != y.isCastling || x.isPawnDoublePush != y.isPawnDoublePush) {
      return false;
    }
  }
  return true;
}

struct TestPosition {
  std::string name;
  std::string fen;
};

const std::vector<TestPosition> TEST_POSITIONS = {
    {"starting position", ""},
    {"kiwipete",
     "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"},
    {"position 3",
     "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1"},
    {"en passant", "4k3/8/8/3pP4/8/8/8/4K3 w - d6 0 1"},
    {"promotion", "4k3/P7/8/8/8/8/8/4K3 w - - 0 1"},
    {"promotion capture", "1n2k3/P7/8/8/8/8/8/4K3 w - - 0 1"},
    {"black to move", "8/8/8/8/8/8/5k2/4K2R b K - 0 1"},
};

} // namespace

TEST_CASE("makeMove + undoMove restore the full engine state",
          "[makeundo]") {
  for (const TestPosition &pos : TEST_POSITIONS) {
    INFO("position: " << pos.name);

    HerodotusEngine engine;
    if (pos.fen.empty()) {
      engine.initialise();
    } else {
      engine.loadFEN(pos.fen);
    }

    const StateSnapshot before = snapshot(engine);
    const std::vector<Move> originalMoves = engine.generateAllLegalMoves();

    REQUIRE_FALSE(originalMoves.empty());

    for (const Move &move : originalMoves) {
      INFO("position: " << pos.name << " | move from=" << (int)move.from.rank
                        << "," << (int)move.from.file << " to="
                        << (int)move.to.rank << "," << (int)move.to.file
                        << " ep=" << move.isEnPassant
                        << " castle=" << move.isCastling
                        << " promo=" << move.promotion.has_value());

      engine.makeMove(move);
      engine.undoMove();

      const StateSnapshot after = snapshot(engine);
      const std::string diff = firstMismatch(before, after);
      INFO("mismatch field: " << diff);
      REQUIRE(diff.empty());

      // Behavioral check: the same legal moves must be generated again.
      REQUIRE(movesEqual(engine.generateAllLegalMoves(), originalMoves));
    }
  }
}
