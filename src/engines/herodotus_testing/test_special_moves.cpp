#include <catch2/catch_test_macros.hpp>
#include "../herodotus/core/HerodotusEngine.hpp"
#include "../herodotus/types/Move.hpp"

#include <vector>

namespace {

template <typename Pred>
const Move *findMove(const std::vector<Move> &moves, Pred pred) {
  for (const Move &m : moves) {
    if (pred(m)) {
      return &m;
    }
  }
  return nullptr;
}

const Move *findMoveFromTo(const std::vector<Move> &moves, Square from,
                           Square to) {
  return findMove(moves, [&](const Move &m) {
    return m.from == from && m.to == to;
  });
}

bool hasKingsideCastle(const std::vector<Move> &moves, Color c) {
  return findMove(moves, [&](const Move &m) {
    return m.isCastling && m.color == c && m.to.file > m.from.file;
  }) != nullptr;
}

bool hasQueensideCastle(const std::vector<Move> &moves, Color c) {
  return findMove(moves, [&](const Move &m) {
    return m.isCastling && m.color == c && m.to.file < m.from.file;
  }) != nullptr;
}

// Replays: 1. e4 e5 2. Nf3 Nf6 3. Be2 d6 -> white kingside castling legal.
void reachCastlingPosition(HerodotusEngine &e) {
  e.initialise();

  std::vector<Move> moves = e.generateAllLegalMoves();
  const Move *e4 = findMoveFromTo(moves, {1, 4}, {3, 4}); // e2-e4
  REQUIRE(e4 != nullptr);
  e.makeMove(*e4);

  moves = e.generateAllLegalMoves();
  const Move *e5 = findMoveFromTo(moves, {6, 4}, {4, 4}); // e7-e5
  REQUIRE(e5 != nullptr);
  e.makeMove(*e5);

  moves = e.generateAllLegalMoves();
  const Move *nf3 = findMoveFromTo(moves, {0, 6}, {2, 5}); // g1-f3
  REQUIRE(nf3 != nullptr);
  e.makeMove(*nf3);

  moves = e.generateAllLegalMoves();
  const Move *nf6 = findMoveFromTo(moves, {7, 6}, {5, 5}); // g8-f6
  REQUIRE(nf6 != nullptr);
  e.makeMove(*nf6);

  moves = e.generateAllLegalMoves();
  const Move *be2 = findMoveFromTo(moves, {0, 5}, {1, 4}); // f1-e2
  REQUIRE(be2 != nullptr);
  e.makeMove(*be2);

  moves = e.generateAllLegalMoves();
  const Move *d6 = findMoveFromTo(moves, {6, 3}, {5, 3}); // d7-d6
  REQUIRE(d6 != nullptr);
  e.makeMove(*d6);
}

} // namespace

TEST_CASE("Castling: not available from starting position", "[castling]") {
  HerodotusEngine e;
  e.initialise();

  std::vector<Move> moves = e.generateAllLegalMoves();
  REQUIRE_FALSE(hasKingsideCastle(moves, Color::WHITE));
  REQUIRE_FALSE(hasQueensideCastle(moves, Color::WHITE));
  REQUIRE_FALSE(hasKingsideCastle(moves, Color::BLACK));
  REQUIRE_FALSE(hasQueensideCastle(moves, Color::BLACK));
}

TEST_CASE("Castling: becomes legal once the path is cleared", "[castling]") {
  HerodotusEngine e;
  reachCastlingPosition(e);

  std::vector<Move> moves = e.generateAllLegalMoves();
  const Move *o_o = findMoveFromTo(moves, {0, 4}, {0, 6}); // e1-g1
  REQUIRE(o_o != nullptr);
  REQUIRE(o_o->isCastling);
  REQUIRE(o_o->piece == Piece::KING);
  REQUIRE_FALSE(hasQueensideCastle(moves, Color::WHITE)); // b1/c1/d1 still blocked
}

TEST_CASE("Castling: illegal when king passes through an attacked square",
          "[castling]") {
  // Black rook g8 attacks g1 -> white cannot castle kingside (through check),
  // but queenside remains legal.
  HerodotusEngine e;
  e.loadFEN("4k1r1/8/8/8/8/8/8/R3K2R w KQkq - 0 1");

  std::vector<Move> moves = e.generateAllLegalMoves();
  REQUIRE_FALSE(hasKingsideCastle(moves, Color::WHITE));
  REQUIRE(hasQueensideCastle(moves, Color::WHITE));
}

TEST_CASE("Castling: illegal when the king is in check", "[castling]") {
  // Black rook e2 checks the white king on e1.
  HerodotusEngine e;
  e.loadFEN("4k3/8/4r3/8/8/8/8/R3K2R w KQkq - 0 1");

  std::vector<Move> moves = e.generateAllLegalMoves();
  REQUIRE_FALSE(hasKingsideCastle(moves, Color::WHITE));
  REQUIRE_FALSE(hasQueensideCastle(moves, Color::WHITE));
}

TEST_CASE("Castling: makeMove places king and rook; undoMove restores",
          "[castling]") {
  HerodotusEngine e;
  reachCastlingPosition(e);

  uint64_t preHash = e.getZobristHash();
  size_t preHistory = e.getHistorySize();

  std::vector<Move> moves = e.generateAllLegalMoves();
  const Move *o_o = findMoveFromTo(moves, {0, 4}, {0, 6});
  REQUIRE(o_o != nullptr);

  e.makeMove(*o_o);

  // King on g1 (bit 6); rook from h1 (bit 7) to f1 (bit 5).
  REQUIRE(e.getPieceBitboard(Color::WHITE, Piece::KING) == (1ULL << 6));
  Bitboard whiteRooks = e.getPieceBitboard(Color::WHITE, Piece::ROOK);
  REQUIRE((whiteRooks & (1ULL << 5)) != 0);
  REQUIRE((whiteRooks & (1ULL << 7)) == 0);
  REQUIRE(e.getMailbox()[6] == Piece::KING);
  REQUIRE(e.getMailbox()[5] == Piece::ROOK);
  REQUIRE(e.getMailbox()[7] == Piece::NUM_PIECES);

  // White castling rights revoked.
  REQUIRE((e.getGameState().castlingRights &
           GameState::WHITE_KINGSIDE_CASTLE) == 0);
  REQUIRE((e.getGameState().castlingRights &
           GameState::WHITE_QUEENSIDE_CASTLE) == 0);

  REQUIRE(e.getGameState().activeColor == Color::BLACK);
  REQUIRE(e.getHistorySize() == preHistory + 1);
  REQUIRE(e.getZobristHash() != preHash);

  e.undoMove();

  REQUIRE(e.getPieceBitboard(Color::WHITE, Piece::KING) == (1ULL << 4));
  Bitboard rooksAfterUndo = e.getPieceBitboard(Color::WHITE, Piece::ROOK);
  REQUIRE((rooksAfterUndo & (1ULL << 7)) != 0);
  REQUIRE((rooksAfterUndo & (1ULL << 5)) == 0);
  REQUIRE(e.getMailbox()[4] == Piece::KING);
  REQUIRE(e.getMailbox()[7] == Piece::ROOK);
  REQUIRE(e.getGameState().activeColor == Color::WHITE);
  REQUIRE(e.getHistorySize() == preHistory);
  REQUIRE(e.getZobristHash() == preHash);
}

TEST_CASE("En passant: generated immediately after a double push",
          "[enpassant]") {
  // White pawn e5, black pawn d5 (just double-pushed), ep target d6.
  HerodotusEngine e;
  e.loadFEN("4k3/8/8/3pP4/8/8/8/4K3 w - d6 0 1");

  std::vector<Move> moves = e.generateAllLegalMoves();
  const Move *ep = findMove(moves, [](const Move &m) {
    return m.isEnPassant && m.from == Square{4, 4} && m.to == Square{5, 3};
  });
  REQUIRE(ep != nullptr);
  REQUIRE(ep->piece == Piece::PAWN);
  REQUIRE(ep->captured == Piece::PAWN);
  REQUIRE(ep->color == Color::WHITE);
}

TEST_CASE("En passant: captured pawn removed on makeMove, restored on undo",
          "[enpassant]") {
  HerodotusEngine e;
  e.loadFEN("4k3/8/8/3pP4/8/8/8/4K3 w - d6 0 1");

  uint64_t preHash = e.getZobristHash();
  std::vector<Move> moves = e.generateAllLegalMoves();
  const Move *ep = findMove(moves, [](const Move &m) {
    return m.isEnPassant && m.to == Square{5, 3};
  });
  REQUIRE(ep != nullptr);

  e.makeMove(*ep);

  // White pawn now on d6; the captured black pawn on d5 is gone.
  REQUIRE(e.getPieceBitboard(Color::WHITE, Piece::PAWN) == (1ULL << (5 * 8 + 3)));
  REQUIRE(e.getPieceBitboard(Color::BLACK, Piece::PAWN) == 0);
  REQUIRE(e.getMailbox()[5 * 8 + 3] == Piece::PAWN);
  REQUIRE(e.getMailbox()[4 * 8 + 3] == Piece::NUM_PIECES); // d5 empty
  REQUIRE(e.getMailbox()[4 * 8 + 4] == Piece::NUM_PIECES); // e5 empty
  REQUIRE_FALSE(e.getGameState().enPassant.has_value());

  e.undoMove();

  REQUIRE(e.getPieceBitboard(Color::WHITE, Piece::PAWN) == (1ULL << (4 * 8 + 4)));
  REQUIRE(e.getPieceBitboard(Color::BLACK, Piece::PAWN) == (1ULL << (4 * 8 + 3)));
  REQUIRE(e.getMailbox()[4 * 8 + 4] == Piece::PAWN);
  REQUIRE(e.getMailbox()[4 * 8 + 3] == Piece::PAWN);
  REQUIRE(e.getGameState().enPassant == Square{5, 3});
  REQUIRE(e.getZobristHash() == preHash);
}

TEST_CASE("En passant: target cleared after a non-ep move", "[enpassant]") {
  HerodotusEngine e;
  e.loadFEN("4k3/8/8/3pP4/8/8/8/4K3 w - d6 0 1");

  std::vector<Move> moves = e.generateAllLegalMoves();
  const Move *ke2 = findMoveFromTo(moves, {0, 4}, {1, 4}); // e1-e2
  REQUIRE(ke2 != nullptr);
  e.makeMove(*ke2);
  REQUIRE_FALSE(e.getGameState().enPassant.has_value());

  // Now black to move: no en passant moves present.
  moves = e.generateAllLegalMoves();
  REQUIRE_FALSE(findMove(moves, [](const Move &m) { return m.isEnPassant; }));
}

TEST_CASE("En passant: pawn double push sets the target square", "[enpassant]") {
  HerodotusEngine e;
  e.initialise();

  std::vector<Move> moves = e.generateAllLegalMoves();
  const Move *e4 = findMoveFromTo(moves, {1, 4}, {3, 4}); // e2-e4
  REQUIRE(e4 != nullptr);
  REQUIRE(e4->isPawnDoublePush);

  e.makeMove(*e4);
  REQUIRE(e.getGameState().enPassant == Square{2, 4}); // e3

  // Black replies with a single push; the ep target disappears.
  moves = e.generateAllLegalMoves();
  const Move *a6 = findMoveFromTo(moves, {6, 0}, {5, 0}); // a7-a6
  REQUIRE(a6 != nullptr);
  REQUIRE_FALSE(a6->isPawnDoublePush);
  e.makeMove(*a6);
  REQUIRE_FALSE(e.getGameState().enPassant.has_value());
}

TEST_CASE("En passant: single push does not set a target", "[enpassant]") {
  HerodotusEngine e;
  e.initialise();

  std::vector<Move> moves = e.generateAllLegalMoves();
  const Move *e3 = findMoveFromTo(moves, {1, 4}, {2, 4}); // e2-e3
  REQUIRE(e3 != nullptr);
  REQUIRE_FALSE(e3->isPawnDoublePush);

  e.makeMove(*e3);
  REQUIRE_FALSE(e.getGameState().enPassant.has_value());
}

TEST_CASE("Promotion: four promotions generated at the last rank",
          "[promotion]") {
  HerodotusEngine e;
  e.loadFEN("4k3/P7/8/8/8/8/8/4K3 w - - 0 1");

  std::vector<Move> moves = e.generateAllLegalMoves();

  int promotions = 0;
  bool sawKnight = false, sawBishop = false, sawRook = false, sawQueen = false;
  for (const Move &m : moves) {
    if (m.piece == Piece::PAWN && m.from == Square{6, 0} &&
        m.to == Square{7, 0}) {
      REQUIRE(m.promotion.has_value());
      promotions++;
      switch (*m.promotion) {
      case Piece::KNIGHT:
        sawKnight = true;
        break;
      case Piece::BISHOP:
        sawBishop = true;
        break;
      case Piece::ROOK:
        sawRook = true;
        break;
      case Piece::QUEEN:
        sawQueen = true;
        break;
      default:
        FAIL("promotion to invalid piece");
      }
    }
  }
  REQUIRE(promotions == 4);
  REQUIRE((sawKnight && sawBishop && sawRook && sawQueen));
}

TEST_CASE("Promotion: capture-promotion keeps the captured piece",
          "[promotion]") {
  // White pawn a7, black knight b8.
  HerodotusEngine e;
  e.loadFEN("1n2k3/P7/8/8/8/8/8/4K3 w - - 0 1");

  std::vector<Move> moves = e.generateAllLegalMoves();

  int capturePromotions = 0;
  int quietPromotions = 0;
  for (const Move &m : moves) {
    if (m.piece != Piece::PAWN || m.from != Square{6, 0} ||
        !m.promotion.has_value()) {
      continue;
    }
    if (m.to == Square{7, 1}) { // b8 capture
      REQUIRE(m.captured == Piece::KNIGHT);
      capturePromotions++;
    } else if (m.to == Square{7, 0}) { // a8 quiet push
      REQUIRE_FALSE(m.captured.has_value());
      quietPromotions++;
    }
  }
  REQUIRE(capturePromotions == 4);
  REQUIRE(quietPromotions == 4);
}

TEST_CASE("Promotion: makeMove and undoMove round-trip", "[promotion]") {
  HerodotusEngine e;
  e.loadFEN("4k3/P7/8/8/8/8/8/4K3 w - - 0 1");

  uint64_t preHash = e.getZobristHash();
  std::vector<Move> moves = e.generateAllLegalMoves();
  const Move *promo = findMove(moves, [](const Move &m) {
    return m.from == Square{6, 0} && m.to == Square{7, 0} &&
           m.promotion == Piece::QUEEN;
  });
  REQUIRE(promo != nullptr);

  e.makeMove(*promo);
  REQUIRE(e.getPieceBitboard(Color::WHITE, Piece::PAWN) == 0);
  REQUIRE(e.getPieceBitboard(Color::WHITE, Piece::QUEEN) == (1ULL << (7 * 8)));
  REQUIRE(e.getMailbox()[7 * 8] == Piece::QUEEN);
  REQUIRE(e.getMailbox()[6 * 8] == Piece::NUM_PIECES);

  e.undoMove();
  REQUIRE(e.getPieceBitboard(Color::WHITE, Piece::PAWN) == (1ULL << (6 * 8)));
  REQUIRE(e.getPieceBitboard(Color::WHITE, Piece::QUEEN) == 0);
  REQUIRE(e.getMailbox()[6 * 8] == Piece::PAWN);
  REQUIRE(e.getMailbox()[7 * 8] == Piece::NUM_PIECES);
  REQUIRE(e.getZobristHash() == preHash);
}
