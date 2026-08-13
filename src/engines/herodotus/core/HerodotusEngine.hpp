#pragma once

#include "../types/Bitboard.hpp"
#include "../types/GameState.hpp"
#include "../types/Move.hpp"
#include "../types/Types.hpp"
#include "../types/BoardMemento.hpp"
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

/**
 * @file HerodotusEngine.hpp
 * @brief Core chess engine for the Herodotus chess system.
 *
 * The Herodotus Engine is a bitboard-based chess engine that maintains
 * chess positions using 64-bit bitboards for efficient move generation,
 * validation, and position analysis.
 *
 * Architecture:
 * - Uses separate bitboards for each piece type and color
 * - Maintains game state (castling rights, en passant, move counters)
 * - Provides position setup, printing, and analysis functionality
 *
 * @example
 * ```cpp
 * HerodotusEngine engine;
 * engine.initialise();           // Set up starting position
 * engine.printBoardState();      // Display the current position
 * ```
 */

class HerodotusEngine;

namespace MoveGen {
void generatePseudoLegalMoves(HerodotusEngine &engine, std::vector<Move> &out);
bool isKingChecked(HerodotusEngine &engine, Color side);
std::vector<Move> pseudoToLegalMoves(HerodotusEngine &engine,
                                     const std::vector<Move> pseudoLegalMoves);

bool validateMove(HerodotusEngine &engine, Move move);

} // namespace MoveGen

/**
 * @class HerodotusEngine
 * @brief Main chess engine class for position representation and manipulation.
 *
 * The engine maintains:
 * - A 12-bitboard array: 6 piece types × 2 colors
 * - Complete game state (whose turn, castling rights, en passant)
 * - Utility functions for position analysis and display
 *
 * The internal bitboard layout uses little-endian encoding where bit 0
 * represents the a1 square, bit 7 represents h1, bit 8 represents a2, etc.
 *
 * @invariant pieces[color][piece] contains exactly one bit for each piece of
 * that type
 * @invariant gameState is kept in sync with piece positions
 */
class HerodotusEngine {
private:
  // ===== Data Members =====

  /**
   * @brief Bitboards for all pieces.
   *
   * Two-dimensional array of bitboards:
   * - First dimension: Color (0 = white, 1 = black)
   * - Second dimension: Piece type (0 = pawn, ..., 5 = king)
   *
   * Each bit in pieces[color][piece] represents a piece of that type and color.
   * A bit set to 1 means a piece occupies that square.
   */
  PositionBitboards pieces;

  /// Current game state (whose turn, castling rights, move counters, etc.)
  GameState gameState;

  std::stack<BoardMemento> history;

  /**
   * @brief Mailbox array for O(1) piece lookup by square index.
   *
   * mailbox[sq] contains the Piece type at that square, or NUM_PIECES if empty.
   * Kept in sync with the bitboard arrays by syncMailbox().
   */
  Mailbox mailbox;

  friend void MoveGen::generatePseudoLegalMoves(HerodotusEngine &engine,
                                                std::vector<Move> &);
  friend bool MoveGen::isKingChecked(HerodotusEngine &engine, Color side);
  friend std::vector<Move>
  pseudoToLegalMoves(HerodotusEngine &engine,
                     const std::vector<Move> pseudoLegalMoves);

  friend bool MoveGen::validateMove(HerodotusEngine &engine, Move move);

  uint64_t zobristHash = 0;
  std::unordered_map<uint64_t, int> boardStateHashCount;

  // ===== Private Methods =====

  Bitboard getWhitePieces();
  Bitboard getBlackPieces();
  Bitboard getAllPieces();

  /**
   * @brief Rebuilds the mailbox from the bitboard arrays.
   *
   * Iterates every piece bitboard and sets mailbox[sq] for each occupied
   * square. Empty squares are set to Piece::NUM_PIECES.
   *
   * @post mailbox[sq] == pieces[color][piece] is consistent for all squares.
   */
  void syncMailbox();

  /**
   * @brief Computes the initial Zobrist hash for the current position.
   *
   * XORs the hash for every piece, plus the castling rights and side-to-move
   * indicators. Must be called after the board and gameState are set up
   * (i.e. at the end of initialise()).
   */
  void initialiseHash();

  /**
   * @brief Sets up the default starting position for a chess game.
   *
   * Initializes all piece bitboards to represent the standard chess starting
   * position (after 0 moves):
   * - Pawns on rank 2 (white) and rank 7 (black)
   * - Other pieces arranged in their initial configuration
   *
   * The gameState is NOT modified by this method; call initialise() for full
   * setup.
   *
   * @post All piece bitboards contain the starting position
   * @see initialise()
   */
  void setupStartingPosition();

  /**
   * @brief Clears all piece bitboards to represent an empty board.
   *
   * Sets all bitboards in the pieces array to 0, indicating no pieces
   * occupy any square.
   *
   * The gameState is NOT modified by this method.
   *
   * @post All pieces[i][j] == 0
   */
  void clearBoard();

public:
  /**
   * @brief Initializes the engine to the default starting chess position.
   *
   * This is the main initialization method that should be called once at engine
   * startup. It performs:
   * 1. Clears all bitboards to ensure a clean state
   * 2. Sets up the standard starting position
   * 3. Initializes game state with proper defaults (white to move, all castling
   *    rights available, no en passant)
   *
   * @post Engine is ready to play from the starting position
   */
  void initialise();

  /**
   * @brief Prints the current board state to standard output.
   *
   * Displays the current position in a human-readable ASCII format:
   * - Uppercase letters for white pieces (P, N, B, R, Q, K)
   * - Lowercase letters for black pieces (p, n, b, r, q, k)
   * - Dots (.) for empty squares
   *
   * The board is printed with rank 8 at the top and rank 1 at the bottom,
   * showing the board from white's perspective.
   */
  void printBoardState();
  // TODO: Formulate away to advance the game state -> makeMove and undoMove
  // 1. Use the ZobristHashing
  // 2. Keep track of game state

  // TODO: The hardest part: move pruncing and move decisions, done after the
  // above two. This part represents the actual engine (apart from move
  // generation ofc).
  //
  void makeMove(Move move);

  void undoMove(void);

  std::vector<Move> generateAllLegalMoves();

  /**
   * @brief Loads a position from a FEN string.
   *
   * Parses the six standard FEN fields (piece placement, active color, castling
   * rights, en passant target, halfmove clock, fullmove number) and rebuilds
   * the bitboards, mailbox, game state, and Zobrist hash accordingly.
   *
   * @param fen A Forsyth-Edwards Notation string describing a chess position.
   */
  void loadFEN(const std::string &fen);

  // ===== Public Accessors =====

  /// @brief Returns the current game state (whose turn, castling, en passant).
  const GameState &getGameState() const { return gameState; }

  /// @brief Returns the current Zobrist hash of the position.
  uint64_t getZobristHash() const { return zobristHash; }

  /// @brief Returns the mailbox (piece type per square index).
  const Mailbox &getMailbox() const { return mailbox; }

  /// @brief Returns the bitboard for a given piece type and color.
  Bitboard getPieceBitboard(Color color, Piece piece) const {
    return pieces[color][piece];
  }

  /// @brief Returns the number of moves currently pushed on the history stack.
  size_t getHistorySize() const { return history.size(); }
};
