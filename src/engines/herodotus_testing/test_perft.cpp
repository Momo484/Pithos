#include <catch2/catch_test_macros.hpp>
#include "../herodotus/core/HerodotusEngine.hpp"


#include <chrono>
#include <iostream>

// 1. The Recursive Perft Function
uint64_t runPerft(HerodotusEngine &engine, int depth) {
  if (depth == 0) {
    return 1ULL;
  }

  uint64_t nodes = 0;
  const std::vector<Move> &moves = engine.generateAllLegalMoves();

  for (const Move &move : moves) {
    engine.makeMove(move);
    nodes += runPerft(engine, depth - 1);
    engine.undoMove();
  }

  return nodes;
}

// 2. The Benchmarking Wrapper
void runPerftWithTiming(int depth, uint64_t expectedNodes) {
  HerodotusEngine engine;
  engine.initialise();

  // Start the clock
  auto start_time = std::chrono::high_resolution_clock::now();

  // Run the computation
  uint64_t actualNodes = runPerft(engine, depth);

  // Stop the clock
  auto end_time = std::chrono::high_resolution_clock::now();

  std::chrono::duration<double, std::ratio<1>> time_span =
      end_time - start_time;
  double seconds = time_span.count();

  // Calculate Nodes Per Second (NPS)
  uint64_t nps =
      (seconds > 0.0) ? static_cast<uint64_t>(actualNodes / seconds) : 0;

  std::cout << "Depth: " << depth << " | Nodes: " << actualNodes
            << " | Time: " << seconds << "s"
            << " | NPS: " << nps << "\n";

  // Catch2 Assertion
  REQUIRE(actualNodes == expectedNodes);
}

// 2b. The FEN-based Benchmarking Wrapper
void runPerftFromFENWithTiming(const std::string &fen, int depth,
                               uint64_t expectedNodes) {
  HerodotusEngine engine;
  engine.loadFEN(fen);

  auto start_time = std::chrono::high_resolution_clock::now();
  uint64_t actualNodes = runPerft(engine, depth);
  auto end_time = std::chrono::high_resolution_clock::now();

  std::chrono::duration<double, std::ratio<1>> time_span =
      end_time - start_time;
  double seconds = time_span.count();

  uint64_t nps =
      (seconds > 0.0) ? static_cast<uint64_t>(actualNodes / seconds) : 0;

  std::cout << "Depth: " << depth << " | Nodes: " << actualNodes
            << " | Time: " << seconds << "s"
            << " | NPS: " << nps << "\n";

  REQUIRE(actualNodes == expectedNodes);
}

// 3. The Catch2 Test Cases
TEST_CASE("Perft: Starting Position", "[perft]") {
  std::cout << "\n--- Starting Position Perft ---\n";

  SECTION("Depth 1") { runPerftWithTiming(1, 20); }
  SECTION("Depth 2") { runPerftWithTiming(2, 400); }
  SECTION("Depth 3") { runPerftWithTiming(3, 8902); }
  SECTION("Depth 4") { runPerftWithTiming(4, 197281); }
  SECTION("Depth 5") { runPerftWithTiming(5, 4865609); }
  SECTION("Depth 6") { runPerftWithTiming(6, 119060324); }
}

TEST_CASE("Perft: Kiwipete (position 2)", "[perft]") {
  std::cout << "\n--- Kiwipete Perft ---\n";
  const std::string kiwipete =
      "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";

  SECTION("Depth 1") { runPerftFromFENWithTiming(kiwipete, 1, 48); }
  SECTION("Depth 2") { runPerftFromFENWithTiming(kiwipete, 2, 2039); }
  SECTION("Depth 3") { runPerftFromFENWithTiming(kiwipete, 3, 97862); }
  SECTION("Depth 4") { runPerftFromFENWithTiming(kiwipete, 4, 4085603); }
}

TEST_CASE("Perft: Position 3", "[perft]") {
  std::cout << "\n--- Position 3 Perft ---\n";
  const std::string position3 =
      "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";

  SECTION("Depth 1") { runPerftFromFENWithTiming(position3, 1, 6); }
  SECTION("Depth 2") { runPerftFromFENWithTiming(position3, 2, 264); }
  SECTION("Depth 3") { runPerftFromFENWithTiming(position3, 3, 9467); }
  SECTION("Depth 4") { runPerftFromFENWithTiming(position3, 4, 422333); }
}

TEST_CASE("Perft: Position 4", "[perft]") {
  std::cout << "\n--- Position 4 Perft ---\n";
  const std::string position4 =
      "r3k2r/1b4bq/8/8/8/8/7B/R3K2R w KQkq - 0 1";

  SECTION("Depth 1") { runPerftFromFENWithTiming(position4, 1, 26); }
  SECTION("Depth 2") { runPerftFromFENWithTiming(position4, 2, 1141); }
  SECTION("Depth 3") { runPerftFromFENWithTiming(position4, 3, 27826); }
  SECTION("Depth 4") { runPerftFromFENWithTiming(position4, 4, 1274206); }
}
