#include "../core/Board.hpp"
#include "../core/Move.hpp"
#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <iostream>
#include <vector>

// 1. The Recursive Perft Function
uint64_t runPerft(Board &board, int depth, bool isWhiteTurn) {
  if (depth == 0) {
    return 1ULL;
  }

  uint64_t nodes = 0;
  const std::vector<Move> &moves = board.generateAllLegalMoves(isWhiteTurn);

  for (const Move &move : moves) {
    board.makeMove(move);
    nodes += runPerft(board, depth - 1, !isWhiteTurn);
    board.undoMove();
  }

  return nodes;
}

// 2. The Benchmarking Wrapper
void runPerftWithTiming(int depth, uint64_t expectedNodes) {
  Board board;
  bool isWhiteTurn = true;

  // Start the clock
  auto start_time = std::chrono::high_resolution_clock::now();

  // Run the computation
  uint64_t actualNodes = runPerft(board, depth, isWhiteTurn);

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

// 3. The Catch2 Test Cases
TEST_CASE("Perft: Starting Position", "[perft]") {
  std::cout << "\n--- Starting Position Perft ---\n";

  SECTION("Depth 1") { runPerftWithTiming(1, 20); }
  SECTION("Depth 2") { runPerftWithTiming(2, 400); }
  SECTION("Depth 3") { runPerftWithTiming(3, 8902); }
  SECTION("Depth 4") { runPerftWithTiming(4, 197281); }
  SECTION("Depth 5") { runPerftWithTiming(5, 4865609); }
}
