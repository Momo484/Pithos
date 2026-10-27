#include "core/HerodotusEngine.hpp"
#include <cstddef>
#include <iostream>

// Demo entry point: set up the starting position, print the board and list the
// legal moves from it.
int main() {
  // Create a new engine instance.
  HerodotusEngine engine;

  // Initialise to the standard starting chess position.
  engine.initialise();

  // Display the current board state, then the legal move count.
  engine.printBoardState();
  const std::vector<Move> moves = engine.generateAllLegalMoves();
  std::cout << moves.size() << std::endl;
  for (Move move : moves) {
    std::cout << move.piece << std::endl;
  }

  return 0;
}
