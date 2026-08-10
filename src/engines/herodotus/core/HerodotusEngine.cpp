#include "HerodotusEngine.hpp"
#include <iostream>
#include <vector>

/**
 * @file HerodotusEngine.cpp
 * @brief Implementation of the Herodotus chess engine core functionality.
 *
 * This file contains the implementation of position setup, board clearing,
 * piece occupancy queries, and board printing functionality.
 */

void HerodotusEngine::setupStartingPosition() {
  // Initialize the board with pieces in their starting positions
  // White pieces on ranks 1-2 (indices 0-1), black pieces on ranks 7-8 (indices 6-7)

  // White pawns: all 8 pawns on rank 2 (bits 8-15)
  pieces[Color::WHITE][Piece::PAWN] = 0x000000000000FF00ULL;

  // White knights: b1 (bit 1) and g1 (bit 6)
  pieces[Color::WHITE][Piece::KNIGHT] = 0x0000000000000042ULL;

  // White bishops: c1 (bit 2) and f1 (bit 5)
  pieces[Color::WHITE][Piece::BISHOP] = 0x0000000000000024ULL;

  // White rooks: a1 (bit 0) and h1 (bit 7)
  pieces[Color::WHITE][Piece::ROOK] = 0x0000000000000081ULL;

  // White queen: d1 (bit 3)
  pieces[Color::WHITE][Piece::QUEEN] = 0x0000000000000008ULL;

  // White king: e1 (bit 4)
  pieces[Color::WHITE][Piece::KING] = 0x0000000000000010ULL;

  // Black pawns: all 8 pawns on rank 7 (bits 48-55)
  pieces[Color::BLACK][Piece::PAWN] = 0x00FF000000000000ULL;

  // Black knights: b8 (bit 57) and g8 (bit 62)
  pieces[Color::BLACK][Piece::KNIGHT] = 0x4200000000000000ULL;

  // Black bishops: c8 (bit 58) and f8 (bit 61)
  pieces[Color::BLACK][Piece::BISHOP] = 0x2400000000000000ULL;

  // Black rooks: a8 (bit 56) and h8 (bit 63)
  pieces[Color::BLACK][Piece::ROOK] = 0x8100000000000000ULL;

  // Black queen: d8 (bit 59)
  pieces[Color::BLACK][Piece::QUEEN] = 0x0800000000000000ULL;

  // Black king: e8 (bit 60)
  pieces[Color::BLACK][Piece::KING] = 0x1000000000000000ULL;
}

void HerodotusEngine::clearBoard() {
  for (auto& colorArray : pieces) {
    for (auto& bb : colorArray) {
      bb = 0ULL;
    }
  }
}

Bitboard HerodotusEngine::getWhitePieces() {
  // Union all white piece bitboards to get occupancy
  return pieces[Color::WHITE][Piece::PAWN] |
         pieces[Color::WHITE][Piece::BISHOP] |
         pieces[Color::WHITE][Piece::KING] |
         pieces[Color::WHITE][Piece::KNIGHT] |
         pieces[Color::WHITE][Piece::ROOK] | pieces[Color::WHITE][Piece::QUEEN];
}

Bitboard HerodotusEngine::getBlackPieces() {
  // Union all black piece bitboards to get occupancy
  return pieces[Color::BLACK][Piece::PAWN] |
         pieces[Color::BLACK][Piece::BISHOP] |
         pieces[Color::BLACK][Piece::KING] |
         pieces[Color::BLACK][Piece::KNIGHT] |
         pieces[Color::BLACK][Piece::ROOK] | pieces[Color::BLACK][Piece::QUEEN];
}

Bitboard HerodotusEngine::getAllPieces() {
  // Union both white and black pieces to get total occupancy
  return getWhitePieces() | getBlackPieces();
}

void HerodotusEngine::initialise() {
  // Clear the board to ensure clean state
  clearBoard();

  // Set up starting position
  setupStartingPosition();

  // Initialize game state to starting values
  gameState.activeColor = Color::WHITE;
  gameState.castlingRights = 0x0F;  // All castling rights available
  gameState.halfMoveClock = 0;
  gameState.fullMoveNumber = 1;
  gameState.enPassant = std::nullopt;

  // Build the mailbox from the bitboard arrays
  syncMailbox();
}

void HerodotusEngine::syncMailbox() {
  for (auto& entry : mailbox) {
    entry = Piece::NUM_PIECES;
  }
  for (int color = 0; color < Color::NUM_COLORS; color++) {
    for (int pieceType = 0; pieceType < Piece::NUM_PIECES; pieceType++) {
      Bitboard bb = pieces[color][pieceType];
      while (bb) {
        int sq = __builtin_ctzll(bb);
        mailbox[sq] = static_cast<Piece>(pieceType);
        bb &= bb - 1;
      }
    }
  }
}

void HerodotusEngine::printBoardState() {
  // Create a 2D grid to represent the board for printing
  std::vector<std::vector<char>> board(8, std::vector<char>(8, '.'));

  // Iterate through each side (white, black) and piece type
  for (int side = 0; side < Color::NUM_COLORS; side++) {
    bool isWhite = (side == Color::WHITE) ? true : false;

    for (int piece = 0; piece < Piece::NUM_PIECES; piece++) {
      // Determine the character representation for this piece
      char pieceChar = 'k';  // Default to king, overridden below
      if (piece == Piece::PAWN) {
        pieceChar = 'p';
      } else if (piece == Piece::BISHOP) {
        pieceChar = 'b';
      } else if (piece == Piece::KNIGHT) {
        pieceChar = 'n';
      } else if (piece == Piece::ROOK) {
        pieceChar = 'r';
      } else if (piece == Piece::QUEEN) {
        pieceChar = 'q';
      }

      // Convert to uppercase for white pieces
      if (isWhite) {
        pieceChar = std::toupper(pieceChar);
      }

      // Place each piece of this type on the board
      Bitboard bb = pieces[side][piece];
      while (bb != 0) {
        // Find the position of the next set bit
        int offset = std::__countr_zero(bb);

        // Remove that bit from the bitboard
        bb ^= (1ULL << offset);

        // Convert bit offset to (rank, file) coordinates
        int currRank = offset / 8;
        int currFile = offset % 8;

        // Place the piece on the board grid
        // Note: board[7-currRank] to flip vertically so rank 8 appears at top
        board[7 - currRank][currFile] = pieceChar;
      }
    }
  }

  // Print the board
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      std::cout << board[i][j] << " ";
    }
    std::cout << std::endl;
  }
}



