#pragma once
#include "BoardMemento.hpp"
#include "Pieces/Piece.hpp"
#include "Square.hpp"
#include <cstdint>
#include <memory>
#include <stack>
#include <string>
#include <unordered_map>

class Piece;

class Board {
private:
  char squares[8][8];
  // we keep static peice objects for logic calls
  std::unique_ptr<Piece> pieceSingletons[12];

  Square epTarget = {-1, -1};
  CastlingRights castling{};
  std::stack<BoardMemento> history;
  Square blackKingPos;
  Square whiteKingPos;

  int halfMoveClock = 0;
  int fullMoveClock = 1;

  // new boardStateHashCount, keeps track of previously seen board states with
  // a zobristHash.
  uint64_t zobristHash = 0;
  std::unordered_map<uint64_t, int> boardStateHashCount;

  /**
   * @brief Takes a list of pseudoLegal moves and filters it, by determining
   * which moves are legal This is acheived by naivly making the move, checking
   * if the current sides king is in check Then undoing the move, given the
   * king's status, the move is either legal (unchecked) or filtered out
   * @return A list of legal moves that are playable.
   */
  std::vector<Move> pseudoToLegalMoves(const std::vector<Move> moves);

  /**
   * @brief Naivly tries to revoke the castling rights of the piece at the
   * square in question
   */
  void revokeCastlingRights(Square sq);

public:
  Board();
  ~Board() = default;

  /**
   * @brief Clears the board, and places all pieces in their starting position.
   */
  void setupStartingPosition();

  /**
   * @brief finds and returns a raw pointer to the piece at the given square.
   * @return A raw pointer to a piece at the square passed in by the parameter.
   */
  Piece *getPieceAt(Square sq) const;

  /**
   * @brief returns the character of the peice at the particular square on the
   * board
   * @return a character of the piece at square.
   */
  char getSquare(Square sq) const { return squares[sq.y][sq.x]; };

  /**
   * @brief Helper function that determines if a given square is on the chess
   * board
   * @return true if the square is on the board
   * @return false if the square is not on the baord.
   */
  static bool onBoard(Square sq);

  Square getEpTarget() const { return epTarget; };
  CastlingRights getCastlingRights() const { return castling; }

  /**
   * @brief Checks if any given pseudolegal move is valid, by making the move,
   * checking the condition of the player in turn's king and undoing the move
   * @return true if the move is valid.
   * @return false if the move isn not valid.
   */
  bool validateMove(Move move);
  /**
   * @brief Given a move, makes the move on the board, taking into account the
   * type of move it was also saves the previous state of the board in a
   * snapshot, so the move history may be replayed
   */
  void makeMove(Move move);
  /**
   * @brief takes the most recent move in the history stack and undo's it,
   * returning the board and game to it's previous state.
   */
  void undoMove();

  /**
   * @brief Through a series of spot and raycast checking determines if the
   * given player's king is in check
   * @return True if the king is in check
   * @return False if the king is not in check
   */
  const bool isKingChecked(bool isWhite);
  /**
   * @brief For a given player, generates all the legal moves that that player
   * may make, without putting their king in check.
   * @return A list of moves that are legal to make at the current point in the
   * game.
   */
  std::vector<Move> generateAllLegalMoves(bool isWhiteTurn);
  /**
   * @brief Scans the board state board for any repeated board states, if the
   * count is every >= 3, return true
   * @return True if a board state has been repeated three times
   * @return False if a board state has not been repeated three times.
   */
  bool checkThreeFoldRepitition();

  void clearEpTarget();
  void setEpTarget(Square sq);

  /**
   * @return The FEN notation string of the given board state, (FULL).
   */
  std::string generateFEN();

  /**
   * @brief Get the current zobrist hash of the board state
   *
   */
  uint64_t getZobristHash() const { return zobristHash; }
};
