#include "HerodotusEngine.hpp"
#include "hash/ZobristHash.hpp"
#include "types/GameState.hpp"
#include "utils/MagicBitboards.hpp"
#include <cassert>
#include <cctype>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
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
  // White pieces on ranks 1-2 (indices 0-1), black pieces on ranks 7-8 (indices
  // 6-7)

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
  for (auto &colorArray : pieces) {
    for (auto &bb : colorArray) {
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
  gameState.castlingRights = 0x0F; // All castling rights available
  gameState.halfMoveClock = 0;
  gameState.fullMoveNumber = 1;
  gameState.enPassant = std::nullopt;

  // Build the mailbox from the bitboard arrays
  syncMailbox();

  // Compute the Zobrist hash for the current position
  initialiseHash();

  // initialise magic bitboard
  MagicBitboards::initMagicBitboards();
}

void HerodotusEngine::loadFEN(const std::string &fen) {
  clearBoard();

  gameState = GameState{};
  gameState.activeColor = Color::WHITE;
  gameState.castlingRights = 0;
  gameState.halfMoveClock = 0;
  gameState.fullMoveNumber = 1;
  gameState.enPassant = std::nullopt;

  std::istringstream stream(fen);
  std::string boardField, colorField, castlingField, epField, halfField,
      fullField;
  stream >> boardField >> colorField >> castlingField >> epField >> halfField >>
      fullField;

  // 1. Piece placement: rank 8 first, '/' separated.
  int rank = 7;
  int file = 0;
  for (char ch : boardField) {
    if (ch == '/') {
      rank--;
      file = 0;
    } else if (std::isdigit(static_cast<unsigned char>(ch))) {
      file += ch - '0';
    } else {
      bool isWhite = std::isupper(static_cast<unsigned char>(ch));
      Piece piece;
      switch (std::tolower(static_cast<unsigned char>(ch))) {
      case 'p':
        piece = Piece::PAWN;
        break;
      case 'n':
        piece = Piece::KNIGHT;
        break;
      case 'b':
        piece = Piece::BISHOP;
        break;
      case 'r':
        piece = Piece::ROOK;
        break;
      case 'q':
        piece = Piece::QUEEN;
        break;
      case 'k':
        piece = Piece::KING;
        break;
      default:
        piece = Piece::NUM_PIECES;
        break;
      }
      pieces[isWhite ? Color::WHITE : Color::BLACK][piece] |=
          (1ULL << (rank * 8 + file));
      file++;
    }
  }

  // 2. Active color.
  if (colorField == "b") {
    gameState.activeColor = Color::BLACK;
  }

  // 3. Castling rights.
  gameState.castlingRights = 0;
  for (char ch : castlingField) {
    switch (ch) {
    case 'K':
      gameState.castlingRights |= GameState::WHITE_KINGSIDE_CASTLE;
      break;
    case 'Q':
      gameState.castlingRights |= GameState::WHITE_QUEENSIDE_CASTLE;
      break;
    case 'k':
      gameState.castlingRights |= GameState::BLACK_KINGSIDE_CASTLE;
      break;
    case 'q':
      gameState.castlingRights |= GameState::BLACK_QUEENSIDE_CASTLE;
      break;
    default:
      break;
    }
  }

  // 4. En passant target square.
  gameState.enPassant = std::nullopt;
  if (epField != "-" && epField.size() == 2) {
    int epFile = epField[0] - 'a';
    int epRank = epField[1] - '1';
    if (epFile >= 0 && epFile < 8 && epRank >= 0 && epRank < 8) {
      gameState.enPassant =
          Square{static_cast<std::uint8_t>(epRank),
                 static_cast<std::uint8_t>(epFile)};
    }
  }

  // 5. Move counters.
  gameState.halfMoveClock = halfField.empty() ? 0 : std::stoi(halfField);
  gameState.fullMoveNumber = fullField.empty() ? 1 : std::stoi(fullField);

  syncMailbox();
  initialiseHash();
  MagicBitboards::initMagicBitboards();
}

void HerodotusEngine::initialiseHash() {
  zobristHash = 0;
  ZobristHash::initialise();

  for (int color = 0; color < Color::NUM_COLORS; color++) {
    for (int pieceType = 0; pieceType < Piece::NUM_PIECES; pieceType++) {
      Bitboard bb = pieces[color][pieceType];
      while (bb) {
        int sq = __builtin_ctzll(bb);
        zobristHash ^= ZobristHash::getPieceHash(static_cast<Piece>(pieceType),
                                                 static_cast<Color>(color),
                                                 Square::fromIndex(sq));
        bb &= bb - 1;
      }
    }
  }

  zobristHash ^= ZobristHash::getCastlingHash(
      gameState.castlingRights & gameState.WHITE_KINGSIDE_CASTLE,
      gameState.castlingRights & gameState.WHITE_QUEENSIDE_CASTLE,
      gameState.castlingRights & gameState.BLACK_KINGSIDE_CASTLE,
      gameState.castlingRights & gameState.BLACK_QUEENSIDE_CASTLE);
  zobristHash ^= ZobristHash::getToMoveHash(gameState.activeColor);

  boardStateHashCount.clear();
}

void HerodotusEngine::syncMailbox() {
  for (auto &entry : mailbox) {
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
      char pieceChar = 'k'; // Default to king, overridden below
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

void HerodotusEngine::makeMove(Move move) {
  history.push({move, gameState});
  Square from = move.from;
  Square to = move.to;
  auto sqIdx = [](Square s) { return s.rank * 8 + s.file; };
  Piece movingPiece = mailbox[sqIdx(from)];

  // removing current state from zobrist hash
  zobristHash ^= ZobristHash::getPieceHash(movingPiece, move.color, from);
  zobristHash ^= ZobristHash::getCastlingHash(
      gameState.castlingRights & gameState.WHITE_KINGSIDE_CASTLE,
      gameState.castlingRights & gameState.WHITE_QUEENSIDE_CASTLE,
      gameState.castlingRights & gameState.BLACK_KINGSIDE_CASTLE,
      gameState.castlingRights & gameState.BLACK_QUEENSIDE_CASTLE);

  if (gameState.enPassant.has_value()) {
    zobristHash ^= ZobristHash::getEnPassantHash(*gameState.enPassant);
  }
  // every move toggles the side to move: XOR in whiteToMove on each move
  zobristHash ^= ZobristHash::getToMoveHash(Color::WHITE);
  gameState.enPassant = std::nullopt;

  // the halfmove clock resets after any pawn move or capture
  if (move.piece == Piece::PAWN || move.captured.has_value()) {
    gameState.halfMoveClock = 0;
  }

  if (move.color == Color::BLACK) {
    // we advance the full move number after black's move
    gameState.fullMoveNumber++;
  }

  // we are going to add the movingpiece to the their new position in the hash

  // we no longer bookkeep king position, instead we implictly find it with
  // the king bitboard.

  if (move.isCastling) {
    // castling
    // We need to:
    // remove from square for king (from move)
    int dir = move.to.file > move.from.file ? 1 : -1;
    // removed the king
    pieces[move.color][Piece::KING] = 0;
    // get to square for king (from move)
    pieces[move.color][Piece::KING] = move.to.squareToU64();
    mailbox[sqIdx(from)] = Piece::NUM_PIECES;
    mailbox[sqIdx(to)] = Piece::KING;
    // remove from square for moving rook (derive) and add to correct
    Bitboard rank = move.color == Color::WHITE ? RANK_1 : RANK_8;
    std::uint8_t rankNum = move.color == Color::WHITE ? 0 : 7;
    if (dir == 1) {
      // kingside
      pieces[move.color][Piece::ROOK] &= ~(rank & FILE_H);
      pieces[move.color][Piece::ROOK] |= (rank & FILE_F);
      mailbox[rankNum * 8 + 7] = Piece::NUM_PIECES;
      mailbox[rankNum * 8 + 5] = Piece::ROOK;
      zobristHash ^=
          ZobristHash::getPieceHash(Piece::ROOK, move.color, {rankNum, 7});
      zobristHash ^=
          ZobristHash::getPieceHash(Piece::ROOK, move.color, {rankNum, 5});

    } else {
      // queenside
      pieces[move.color][Piece::ROOK] &= ~(rank & FILE_A);
      pieces[move.color][Piece::ROOK] |= (rank & FILE_D);
      mailbox[rankNum * 8 + 0] = Piece::NUM_PIECES;
      mailbox[rankNum * 8 + 3] = Piece::ROOK;
      zobristHash ^=
          ZobristHash::getPieceHash(Piece::ROOK, move.color, {rankNum, 0});
      zobristHash ^=
          ZobristHash::getPieceHash(Piece::ROOK, move.color, {rankNum, 3});
    }
    zobristHash ^= ZobristHash::getPieceHash(move.piece, move.color, to);
  } else if (move.isEnPassant) {
    // en passant

    // moving the pawn
    pieces[move.color][move.piece] &= ~move.from.squareToU64();
    pieces[move.color][move.piece] |= move.to.squareToU64();
    mailbox[sqIdx(from)] = Piece::NUM_PIECES;
    mailbox[sqIdx(to)] = Piece::PAWN;

    // removing removed pawn
    // same rank as from, same file as to
    Square capturedPawn = {move.from.rank, move.to.file};
    Color other = move.color == Color::WHITE ? Color::BLACK : Color::WHITE;
    pieces[other][Piece::PAWN] &= ~capturedPawn.squareToU64();
    mailbox[sqIdx(capturedPawn)] = Piece::NUM_PIECES;

    // updating zobrist hash accoringly
    zobristHash ^= ZobristHash::getPieceHash(Piece::PAWN, other, capturedPawn);
    zobristHash ^= ZobristHash::getPieceHash(move.piece, move.color, to);
    // a pawn moved

    // en passant square is automatically removed after any move kinda thing,
    // earlier unless this turn was a doble move.

  } else if (move.isPawnDoublePush) {
    // pawn double push (new enPassant square)
    pieces[move.color][move.piece] &= ~move.from.squareToU64();
    pieces[move.color][move.piece] |= move.to.squareToU64();
    mailbox[sqIdx(from)] = Piece::NUM_PIECES;
    mailbox[sqIdx(to)] = Piece::PAWN;
    // the zobrist hashing for from to to has been done already
    int dir = move.color == Color::WHITE ? 1 : -1;
    Square newEnPassant = {static_cast<std::uint8_t>(move.from.rank + dir),
                           move.from.file};
    gameState.enPassant = newEnPassant;
    zobristHash ^= ZobristHash::getEnPassantHash(newEnPassant);
    zobristHash ^= ZobristHash::getPieceHash(move.piece, move.color, to);
  } else if (move.promotion.has_value()) {
    // promotion, check if capture happened too (conflicts with captured
    // scenario so placed before in condition chain)
    pieces[move.color][move.piece] &= ~move.from.squareToU64();
    pieces[move.color][*move.promotion] |= move.to.squareToU64();
    mailbox[sqIdx(from)] = Piece::NUM_PIECES;
    mailbox[sqIdx(to)] = Piece::NUM_PIECES;
    mailbox[sqIdx(to)] = *move.promotion;
    zobristHash ^= ZobristHash::getPieceHash(*move.promotion, move.color, to);
    if (move.captured.has_value()) {
      Color other = move.color == Color::WHITE ? Color::BLACK : Color::WHITE;
      pieces[other][*move.captured] &= ~move.to.squareToU64();
      zobristHash ^= ZobristHash::getPieceHash(*move.captured, other, move.to);
    }
  } else if (move.captured.has_value()) {
    // capture
    pieces[move.color][move.piece] &= ~move.from.squareToU64();
    pieces[move.color][move.piece] |= move.to.squareToU64();
    mailbox[sqIdx(from)] = Piece::NUM_PIECES;
    mailbox[sqIdx(to)] = Piece::NUM_PIECES;
    mailbox[sqIdx(to)] = move.piece;
    zobristHash ^= ZobristHash::getPieceHash(move.piece, move.color, to);

    // captured
    Color other = move.color == Color::WHITE ? Color::BLACK : Color::WHITE;
    pieces[other][*move.captured] &= ~move.to.squareToU64();
    zobristHash ^= ZobristHash::getPieceHash(*move.captured, other, move.to);
  } else {
    // normal move
    pieces[move.color][move.piece] &= ~move.from.squareToU64();
    pieces[move.color][move.piece] |= move.to.squareToU64();
    mailbox[sqIdx(from)] = Piece::NUM_PIECES;
    mailbox[sqIdx(to)] = move.piece;
    zobristHash ^= ZobristHash::getPieceHash(move.piece, move.color, to);
  }

  // castling rights are lost when the king or a corner rook moves, or when a
  // corner rook is captured
  auto revokeCastlingRight = [&](Square sq, Piece p, Color c) {
    if (p == Piece::KING) {
      if (c == Color::WHITE) {
        gameState.castlingRights &= ~(gameState.WHITE_KINGSIDE_CASTLE |
                                      gameState.WHITE_QUEENSIDE_CASTLE);
      } else {
        gameState.castlingRights &= ~(gameState.BLACK_KINGSIDE_CASTLE |
                                      gameState.BLACK_QUEENSIDE_CASTLE);
      }
    } else if (p == Piece::ROOK) {
      switch (sqIdx(sq)) {
      case 0: // a1
        gameState.castlingRights &= ~gameState.WHITE_QUEENSIDE_CASTLE;
        break;
      case 7: // h1
        gameState.castlingRights &= ~gameState.WHITE_KINGSIDE_CASTLE;
        break;
      case 56: // a8
        gameState.castlingRights &= ~gameState.BLACK_QUEENSIDE_CASTLE;
        break;
      case 63: // h8
        gameState.castlingRights &= ~gameState.BLACK_KINGSIDE_CASTLE;
        break;
      default:
        break;
      }
    }
  };
  revokeCastlingRight(move.from, move.piece, move.color);
  if (move.captured.has_value()) {
    Color other = move.color == Color::WHITE ? Color::BLACK : Color::WHITE;
    revokeCastlingRight(move.to, *move.captured, other);
  }

  zobristHash ^= ZobristHash::getCastlingHash(
      gameState.castlingRights & gameState.WHITE_KINGSIDE_CASTLE,
      gameState.castlingRights & gameState.WHITE_QUEENSIDE_CASTLE,
      gameState.castlingRights & gameState.BLACK_KINGSIDE_CASTLE,
      gameState.castlingRights & gameState.BLACK_QUEENSIDE_CASTLE);

  gameState.activeColor =
      move.color == Color::WHITE ? Color::BLACK : Color::WHITE;
  boardStateHashCount[zobristHash]++;
}

void HerodotusEngine::undoMove(void) {
  assert(!history.empty() && "undoMove called with empty history");
  BoardMemento memento = std::move(history.top());
  history.pop();
  auto sqIdx = [](Square s) { return s.rank * 8 + s.file; };
  boardStateHashCount[zobristHash]--;

  const Move &move = memento.move;

  // removing current state from zobristhash
  if (!move.promotion.has_value()) {
    zobristHash ^= ZobristHash::getPieceHash(move.piece, move.color, move.to);
  }
  zobristHash ^= ZobristHash::getCastlingHash(
      gameState.castlingRights & gameState.WHITE_KINGSIDE_CASTLE,
      gameState.castlingRights & gameState.WHITE_QUEENSIDE_CASTLE,
      gameState.castlingRights & gameState.BLACK_KINGSIDE_CASTLE,
      gameState.castlingRights & gameState.BLACK_QUEENSIDE_CASTLE);

  if (gameState.enPassant.has_value()) {
    zobristHash ^= ZobristHash::getEnPassantHash(*gameState.enPassant);
  }
  zobristHash ^= ZobristHash::getToMoveHash(Color::WHITE);

  gameState = memento.state;

  // re-add the en passant hash for the restored (pre-move) position
  if (gameState.enPassant.has_value()) {
    zobristHash ^= ZobristHash::getEnPassantHash(*gameState.enPassant);
  }

  if (move.isCastling) {
    // move the king back
    pieces[move.color][Piece::KING] = 0;
    pieces[move.color][Piece::KING] = move.from.squareToU64();

    mailbox[sqIdx(move.to)] = Piece::NUM_PIECES;
    mailbox[sqIdx(move.from)] = Piece::KING;

    Bitboard rank = move.color == Color::WHITE ? RANK_1 : RANK_8;
    std::uint8_t rankNum = move.color == Color::WHITE ? 0 : 7;
    int dir = move.to.file > move.from.file ? 1 : -1;

    if (dir == 1) {
      // kingside
      pieces[move.color][Piece::ROOK] &= ~(rank & FILE_F);
      pieces[move.color][Piece::ROOK] |= (rank & FILE_H);
      mailbox[rankNum * 8 + 5] = Piece::NUM_PIECES;
      mailbox[rankNum * 8 + 7] = Piece::ROOK;
      zobristHash ^=
          ZobristHash::getPieceHash(Piece::ROOK, move.color, {rankNum, 5});
      zobristHash ^=
          ZobristHash::getPieceHash(Piece::ROOK, move.color, {rankNum, 7});
    } else {
      // queenside
      pieces[move.color][Piece::ROOK] &= ~(rank & FILE_D);
      pieces[move.color][Piece::ROOK] |= (rank & FILE_A);
      mailbox[rankNum * 8 + 3] = Piece::NUM_PIECES;
      mailbox[rankNum * 8 + 0] = Piece::ROOK;
      zobristHash ^=
          ZobristHash::getPieceHash(Piece::ROOK, move.color, {rankNum, 3});
      zobristHash ^=
          ZobristHash::getPieceHash(Piece::ROOK, move.color, {rankNum, 0});
    }
    zobristHash ^= ZobristHash::getPieceHash(move.piece, move.color, move.from);
  } else if (move.isEnPassant) {
    pieces[move.color][move.piece] &= ~move.to.squareToU64();
    pieces[move.color][move.piece] |= move.from.squareToU64();
    mailbox[sqIdx(move.to)] = Piece::NUM_PIECES;
    mailbox[sqIdx(move.from)] = Piece::PAWN;

    Square capturedPawn = {move.from.rank, move.to.file};
    Color other = move.color == Color::WHITE ? Color::BLACK : Color::WHITE;
    pieces[other][Piece::PAWN] |= capturedPawn.squareToU64();
    mailbox[sqIdx(capturedPawn)] = Piece::PAWN;

    zobristHash ^= ZobristHash::getPieceHash(Piece::PAWN, other, capturedPawn);
    zobristHash ^= ZobristHash::getPieceHash(move.piece, move.color, move.from);

  } else if (move.isPawnDoublePush) {
    pieces[move.color][move.piece] &= ~move.to.squareToU64();
    pieces[move.color][move.piece] |= move.from.squareToU64();
    mailbox[sqIdx(move.from)] = Piece::PAWN;
    mailbox[sqIdx(move.to)] = Piece::NUM_PIECES;
    // the zobrist hashing for from to to has been done already
    zobristHash ^= ZobristHash::getPieceHash(move.piece, move.color, move.from);
  } else if (move.promotion.has_value()) {
    pieces[move.color][move.piece] |= move.from.squareToU64();
    pieces[move.color][*move.promotion] &= ~move.to.squareToU64();
    mailbox[sqIdx(move.from)] = Piece::PAWN;
    mailbox[sqIdx(move.to)] = Piece::NUM_PIECES;
    zobristHash ^=
        ZobristHash::getPieceHash(*move.promotion, move.color, move.to);
    zobristHash ^= ZobristHash::getPieceHash(move.piece, move.color, move.from);
    if (move.captured.has_value()) {
      Color other = move.color == Color::WHITE ? Color::BLACK : Color::WHITE;
      pieces[other][*move.captured] |= move.to.squareToU64();
      mailbox[sqIdx(move.to)] = *move.captured;
      zobristHash ^= ZobristHash::getPieceHash(*move.captured, other, move.to);
    }
  } else if (move.captured.has_value()) {
    // capture
    pieces[move.color][move.piece] |= move.from.squareToU64();
    pieces[move.color][move.piece] &= ~move.to.squareToU64();
    mailbox[sqIdx(move.from)] = move.piece;
    mailbox[sqIdx(move.to)] = *move.captured;
    zobristHash ^= ZobristHash::getPieceHash(move.piece, move.color, move.from);

    // captured
    Color other = move.color == Color::WHITE ? Color::BLACK : Color::WHITE;
    pieces[other][*move.captured] |= move.to.squareToU64();
    zobristHash ^= ZobristHash::getPieceHash(*move.captured, other, move.to);
  } else {
    pieces[move.color][move.piece] |= move.from.squareToU64();
    pieces[move.color][move.piece] &= ~move.to.squareToU64();
    mailbox[sqIdx(move.from)] = move.piece;
    mailbox[sqIdx(move.to)] = Piece::NUM_PIECES;
    zobristHash ^= ZobristHash::getPieceHash(move.piece, move.color, move.from);
  }

  zobristHash ^= ZobristHash::getCastlingHash(
      gameState.castlingRights & gameState.WHITE_KINGSIDE_CASTLE,
      gameState.castlingRights & gameState.WHITE_QUEENSIDE_CASTLE,
      gameState.castlingRights & gameState.BLACK_KINGSIDE_CASTLE,
      gameState.castlingRights & gameState.BLACK_QUEENSIDE_CASTLE);
}

std::vector<Move> HerodotusEngine::generateAllLegalMoves() {
  std::vector<Move> pseudoLegal;
  MoveGen::generatePseudoLegalMoves(*this, pseudoLegal);
  return MoveGen::pseudoToLegalMoves(*this, pseudoLegal);
}
