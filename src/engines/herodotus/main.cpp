/**
 * @file main.cpp
 * @brief Entry point for the Herodotus chess engine.
 *
 * This is the main application entry point that demonstrates basic usage
 * of the Herodotus chess engine by initializing it to the standard starting
 * position and displaying the board.
 */

#include "core/HerodotusEngine.hpp"
#include <cstddef>
#include <iostream>

/**
 * @brief Main entry point for the Herodotus engine.
 *
 * Creates a new HerodotusEngine instance, initializes it to the starting position,
 * and prints the board state to demonstrate the engine is working correctly.
 *
 * @return 0 on successful execution
 */
int main() {
  // Create a new engine instance
  HerodotusEngine engine;

  // Initialize to the standard starting chess position
  engine.initialise();

  // Display the current board state
  engine.printBoardState();
  const std::vector<Move> moves = engine.generateAllLegalMoves();
  std::cout << moves.size() << std::endl;
  for (Move move : moves) {
    std::cout << move.piece << std::endl;
  }

  return 0;
}
