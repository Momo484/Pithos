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

// Bitboard-based chess engine. Pieces live in 12 bitboards, one per colour and
// piece type (bit 0 = a1, rank 0 is rank 1), alongside a mailbox for O(1) piece
// lookup. The engine owns the game state, move history and Zobrist hash, and
// exposes position setup, printing, move make/undo and legal move generation.

class HerodotusEngine;

namespace MoveGen {
void generatePseudoLegalMoves(HerodotusEngine &engine, std::vector<Move> &out);
bool isKingChecked(HerodotusEngine &engine, Color side);
std::vector<Move> pseudoToLegalMoves(HerodotusEngine &engine,
                                     const std::vector<Move> pseudoLegalMoves);

bool validateMove(HerodotusEngine &engine, Move move);

} // namespace MoveGen

class HerodotusEngine {
private:
  // ===== Data Members =====

  // pieces[colour][piece]: every set bit is one piece of that type and colour.
  PositionBitboards pieces;

  // Whose turn it is, castling rights, move counters and en passant target.
  GameState gameState;

  // Mementos pushed by makeMove and popped by undoMove, one per ply.
  std::stack<BoardMemento> history;

  // mailbox[sq] = piece on that square, or NUM_PIECES when it is empty. Kept
  // in sync with the bitboards by syncMailbox().
  Mailbox mailbox;

  // Move generation needs direct access to the bitboards, mailbox and game
  // state, so the free functions in MoveGen are friends.
  friend void MoveGen::generatePseudoLegalMoves(HerodotusEngine &engine,
                                                std::vector<Move> &);
  friend bool MoveGen::isKingChecked(HerodotusEngine &engine, Color side);
  friend std::vector<Move>
  pseudoToLegalMoves(HerodotusEngine &engine,
                     const std::vector<Move> pseudoLegalMoves);

  friend bool MoveGen::validateMove(HerodotusEngine &engine, Move move);

  // Position hash, updated incrementally on every make/undo move.
  uint64_t zobristHash = 0;

  // How many times each position hash has occurred (for repetition detection).
  std::unordered_map<uint64_t, int> boardStateHashCount;

  // ===== Private Methods =====

  // Union of the side's piece bitboards.
  Bitboard getWhitePieces();
  Bitboard getBlackPieces();
  Bitboard getAllPieces();

  // Rebuilds the mailbox from the bitboards; empty squares become NUM_PIECES.
  void syncMailbox();

  // Computes the hash for the current position from scratch: every piece, plus
  // castling rights and side to move. Call after the board and gameState are
  // set up.
  void initialiseHash();

  // Writes the standard starting position into the piece bitboards. Does not
  // touch gameState; initialise() calls this.
  void setupStartingPosition();

  // Clears every piece bitboard. Does not touch gameState.
  void clearBoard();

public:
  // Sets up the starting position and a fresh game state, then builds the
  // mailbox, hash and magic bitboard tables. Call once at startup.
  void initialise();

  // Prints the board as ASCII: uppercase for white, lowercase for black, '.'
  // for empty squares, rank 8 at the top.
  void printBoardState();

  // Applies `move` to the bitboards, mailbox, game state and hash, pushing a
  // memento so undoMove() can restore the position.
  void makeMove(Move move);

  // Restores the position from the most recent memento, including bitboards,
  // mailbox, game state and hash.
  void undoMove(void);

  // Generates all legal moves for the side to move.
  std::vector<Move> generateAllLegalMoves();

  // Rebuilds the engine from a six-field FEN string: placement, side to move,
  // castling, en passant and both move counters.
  void loadFEN(const std::string &fen);

  // ===== Public Accessors =====

  // Current game state (side to move, castling, en passant, counters).
  const GameState &getGameState() const { return gameState; }

  // Current position hash.
  uint64_t getZobristHash() const { return zobristHash; }

  // Piece on each square (NUM_PIECES means empty).
  const Mailbox &getMailbox() const { return mailbox; }

  // Bitboard for one piece type and colour.
  Bitboard getPieceBitboard(Color color, Piece piece) const {
    return pieces[color][piece];
  }

  // Number of moves currently on the history stack.
  size_t getHistorySize() const { return history.size(); }
};
