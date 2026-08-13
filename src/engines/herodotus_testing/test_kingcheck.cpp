#include <catch2/catch_test_macros.hpp>
#include "../herodotus/core/HerodotusEngine.hpp"
#include "../herodotus/movegen/MoveGen.hpp"

TEST_CASE("KingCheck: not in check from the starting position",
          "[kingcheck]") {
  HerodotusEngine e;
  e.initialise();
  REQUIRE_FALSE(MoveGen::isKingChecked(e, Color::WHITE));
  REQUIRE_FALSE(MoveGen::isKingChecked(e, Color::BLACK));
}

TEST_CASE("KingCheck: back-rank rook check on the white king", "[kingcheck]") {
  HerodotusEngine e;
  e.loadFEN("4k3/8/8/8/8/8/8/4K2r w - - 0 1");
  REQUIRE(MoveGen::isKingChecked(e, Color::WHITE));
  REQUIRE_FALSE(MoveGen::isKingChecked(e, Color::BLACK));
}

TEST_CASE("KingCheck: diagonal bishop check on the white king",
          "[kingcheck]") {
  // Bishop b4 -> c3 -> d2 -> e1 (all empty).
  HerodotusEngine e;
  e.loadFEN("4k3/8/8/8/1b6/8/8/4K3 w - - 0 1");
  REQUIRE(MoveGen::isKingChecked(e, Color::WHITE));
}

TEST_CASE("KingCheck: discovered check after a blocker moves", "[kingcheck]") {
  // Black rook a1 and white king e1 on the same rank with the path clear.
  HerodotusEngine e;
  e.loadFEN("4k3/8/8/8/8/8/8/r3K3 w - - 0 1");
  REQUIRE(MoveGen::isKingChecked(e, Color::WHITE));
}

TEST_CASE("KingCheck: double check is still a check", "[kingcheck]") {
  // Black rook e7 (e-file) and black bishop b4 (diagonal) both hit e1.
  HerodotusEngine e;
  e.loadFEN("4k3/4r3/8/8/1b6/8/8/4K3 w - - 0 1");
  REQUIRE(MoveGen::isKingChecked(e, Color::WHITE));
}

TEST_CASE("KingCheck: black king in check from a white rook", "[kingcheck]") {
  HerodotusEngine e;
  e.loadFEN("4k3/8/8/8/8/8/8/4R1K1 b - - 0 1");
  REQUIRE(MoveGen::isKingChecked(e, Color::BLACK));
  REQUIRE_FALSE(MoveGen::isKingChecked(e, Color::WHITE));
}

TEST_CASE("KingCheck: black king in check from a white pawn", "[kingcheck]") {
  // White pawn d6 attacks e7 (black king).
  HerodotusEngine e;
  e.loadFEN("8/4k3/3P4/8/8/8/8/4K3 b - - 0 1");
  REQUIRE(MoveGen::isKingChecked(e, Color::BLACK));
}

TEST_CASE("KingCheck: knight check on the black king", "[kingcheck]") {
  // White knight f6 attacks e8 (black king).
  HerodotusEngine e;
  e.loadFEN("4k3/8/5N2/8/8/8/8/4K3 b - - 0 1");
  REQUIRE(MoveGen::isKingChecked(e, Color::BLACK));
}
