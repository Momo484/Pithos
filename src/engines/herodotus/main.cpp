/**
 * @file main.cpp
 * @brief Entry point for the Herodotus chess engine.
 *
 * This is the main application entry point that demonstrates basic usage
 * of the Herodotus chess engine by initializing it to the standard starting
 * position and displaying the board.
 */

#include "core/HerodotusEngine.hpp"

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

  return 0;
}
