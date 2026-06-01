#include "HerodotusEngine.hpp"
#include <iostream>
#include <vector>

void HerodotusEngine::setupStartingPosition() {
  // Clear or initialize all pieces to 0 first if necessary

  // Rank 1 (0x01) and Rank 2 (0x02)
  pieces[Color::WHITE][Piece::PAWN] = 0x000000000000FF00ULL;   // Rank 2
  pieces[Color::WHITE][Piece::KNIGHT] = 0x0000000000000042ULL; // B1, G1
  pieces[Color::WHITE][Piece::BISHOP] = 0x0000000000000024ULL; // C1, F1
  pieces[Color::WHITE][Piece::ROOK] = 0x0000000000000081ULL;   // A1, H1
  pieces[Color::WHITE][Piece::QUEEN] = 0x0000000000000008ULL;  // D1
  pieces[Color::WHITE][Piece::KING] = 0x0000000000000010ULL;   // E1

  // Rank 7 (0x07) and Rank 8 (0x08)
  pieces[Color::BLACK][Piece::PAWN] = 0x00FF000000000000ULL;   // Rank 7
  pieces[Color::BLACK][Piece::KNIGHT] = 0x4200000000000000ULL; // B8, G8
  pieces[Color::BLACK][Piece::BISHOP] = 0x2400000000000000ULL; // C8, F8
  pieces[Color::BLACK][Piece::ROOK] = 0x8100000000000000ULL;   // A8, H8
  pieces[Color::BLACK][Piece::QUEEN] = 0x0800000000000000ULL;  // D8
  pieces[Color::BLACK][Piece::KING] = 0x1000000000000000ULL;   // E8
}

void HerodotusEngine::clearBoard() {
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 6; j++) {
      pieces[i][j] = 0ULL;
    }
  }
}

U64 HerodotusEngine::getWhitePieces() {
  return pieces[Color::WHITE][Piece::PAWN] |
         pieces[Color::WHITE][Piece::BISHOP] |
         pieces[Color::WHITE][Piece::KING] |
         pieces[Color::WHITE][Piece::KNIGHT] |
         pieces[Color::WHITE][Piece::ROOK] | pieces[Color::WHITE][Piece::QUEEN];
}

U64 HerodotusEngine::getBlackPieces() {
  return pieces[Color::BLACK][Piece::PAWN] |
         pieces[Color::BLACK][Piece::BISHOP] |
         pieces[Color::BLACK][Piece::KING] |
         pieces[Color::BLACK][Piece::KNIGHT] |
         pieces[Color::BLACK][Piece::ROOK] | pieces[Color::BLACK][Piece::QUEEN];
}

U64 HerodotusEngine::getAllPieces() {
  return getWhitePieces() | getBlackPieces();
}

void HerodotusEngine::initialise() {
  clearBoard();
  setupStartingPosition();
}

void HerodotusEngine::printBoardState() {
  std::vector<std::vector<char>> board(8, std::vector<char>(8, '.'));
  // for every bitboard, we add the corresponding character to the squares.
  for (int side = 0; side < 2; side++) {
    bool isWhite = (side == Color::WHITE) ? true : false;
    for (int piece = 0; piece < 6; piece++) {
      // initalise to king, switch if something else.
      char pieceChar = 'k';
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
      // Getting it to the correct case
      if (isWhite) {
        pieceChar = std::toupper(pieceChar);
      }
      // Now we have to add the piece to the char array.
      U64 bb = pieces[side][piece];
      while (bb != 0) {
        // find position
        int offset = std::__countr_zero(bb);
        //  remove that bit
        bb ^= (1ULL << offset);
        int currRank = offset / 8;
        int currFile = offset % 8;
        board[7 - currRank][currFile] = pieceChar;
      }
    }
  }

  // Now we print the board
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      std::cout << board[i][j] << " ";
    }
    std::cout << std::endl;
  }
}
