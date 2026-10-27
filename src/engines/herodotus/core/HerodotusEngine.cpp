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

// Implementation of the engine: position setup, FEN loading, board printing,
// and move make/undo with incremental mailbox and hash updates.

// Writes the standard starting position into the piece bitboards.
void HerodotusEngine::setupStartingPosition() {
  // White starts on ranks 1-2 (board indices 0-1), black on ranks 7-8 (6-7).

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

// Clears every piece bitboard.
void HerodotusEngine::clearBoard() {
  for (auto &colorArray : pieces) {
    for (auto &bb : colorArray) {
      bb = 0ULL;
    }
  }
}

Bitboard HerodotusEngine::getWhitePieces() {
  // Union all white piece bitboards to get occupancy.
  return pieces[Color::WHITE][Piece::PAWN] |
         pieces[Color::WHITE][Piece::BISHOP] |
         pieces[Color::WHITE][Piece::KING] |
         pieces[Color::WHITE][Piece::KNIGHT] |
         pieces[Color::WHITE][Piece::ROOK] | pieces[Color::WHITE][Piece::QUEEN];
}

Bitboard HerodotusEngine::getBlackPieces() {
  // Union all black piece bitboards to get occupancy.
  return pieces[Color::BLACK][Piece::PAWN] |
         pieces[Color::BLACK][Piece::BISHOP] |
         pieces[Color::BLACK][Piece::KING] |
         pieces[Color::BLACK][Piece::KNIGHT] |
         pieces[Color::BLACK][Piece::ROOK] | pieces[Color::BLACK][Piece::QUEEN];
}

Bitboard HerodotusEngine::getAllPieces() {
  // Union both colours to get total occupancy.
  return getWhitePieces() | getBlackPieces();
}

// Sets up the starting position and a fresh game state, then builds the
// mailbox, hash and magic bitboard tables.
void HerodotusEngine::initialise() {
  // Clear the board to ensure a clean state.
  clearBoard();

  // Set up the starting position.
  setupStartingPosition();

  // Reset the game state.
  gameState.activeColor = Color::WHITE;
  gameState.castlingRights = 0x0F; // All castling rights available
  gameState.halfMoveClock = 0;
  gameState.fullMoveNumber = 1;
  gameState.enPassant = std::nullopt;

  // Build the mailbox from the bitboards.
  syncMailbox();

  // Compute the Zobrist hash.
  initialiseHash();

  // Build the magic bitboard attack tables.
  MagicBitboards::initMagicBitboards();
}

// Rebuilds the engine from a six-field FEN string. The parser is lenient: a
// missing side to move or move counters fall back to white to move and move 1.
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

  // 2. Active colour.
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
      gameState.enPassant = Square{static_cast<std::uint8_t>(epRank),
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

// Computes the position hash from scratch: XOR every piece, then the castling
// rights and side to move. Also clears the repetition count map.
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

// Rebuilds the mailbox from the bitboards, setting empty squares to NUM_PIECES.
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

// Prints the board as ASCII: uppercase for white, lowercase for black, '.' for
// empty squares, rank 8 at the top.
void HerodotusEngine::printBoardState() {
  // Grid for printing: board[0] is rank 8.
  std::vector<std::vector<char>> board(8, std::vector<char>(8, '.'));

  // Iterate through each colour and piece type.
  for (int side = 0; side < Color::NUM_COLORS; side++) {
    bool isWhite = (side == Color::WHITE) ? true : false;

    for (int piece = 0; piece < Piece::NUM_PIECES; piece++) {
      // Character for this piece (the king is the default).
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

      // Convert to uppercase for white pieces.
      if (isWhite) {
        pieceChar = std::toupper(pieceChar);
      }

      // Place each piece of this type on the board.
      Bitboard bb = pieces[side][piece];
      while (bb != 0) {
        // Find the position of the next set bit.
        int offset = std::__countr_zero(bb);

        bb ^= (1ULL << offset);

        // Convert bit offset to (rank, file) coordinates.
        int currRank = offset / 8;
        int currFile = offset % 8;

        // board[7 - currRank] flips vertically so rank 8 appears at the top.
        board[7 - currRank][currFile] = pieceChar;
      }
    }
  }

  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      std::cout << board[i][j] << " ";
    }
    std::cout << std::endl;
  }
}

// Applies `move` to the bitboards, mailbox, game state and hash, then pushes a
// memento so undoMove() can restore the position.
void HerodotusEngine::makeMove(Move move) {
  history.push({move, gameState});
  Square from = move.from;
  Square to = move.to;
  auto sqIdx = [](Square s) { return s.rank * 8 + s.file; };
  Piece movingPiece = mailbox[sqIdx(from)];

  // Remove the current state from the hash.
  zobristHash ^= ZobristHash::getPieceHash(movingPiece, move.color, from);
  zobristHash ^= ZobristHash::getCastlingHash(
      gameState.castlingRights & gameState.WHITE_KINGSIDE_CASTLE,
      gameState.castlingRights & gameState.WHITE_QUEENSIDE_CASTLE,
      gameState.castlingRights & gameState.BLACK_KINGSIDE_CASTLE,
      gameState.castlingRights & gameState.BLACK_QUEENSIDE_CASTLE);

  if (gameState.enPassant.has_value()) {
    zobristHash ^= ZobristHash::getEnPassantHash(*gameState.enPassant);
  }
  // Every move toggles the side to move.
  zobristHash ^= ZobristHash::getToMoveHash(Color::WHITE);

  // Clear the en passant target; a double push may set a new one below.
  gameState.enPassant = std::nullopt;

  // The halfmove clock resets after any pawn move or capture.
  if (move.piece == Piece::PAWN || move.captured.has_value()) {
    gameState.halfMoveClock = 0;
  }

  if (move.color == Color::BLACK) {
    // The full move number advances after black's move.
    gameState.fullMoveNumber++;
  }

  if (move.isCastling) {
    // Castling: the king moves two squares and the rook hops over it.
    int dir = move.to.file > move.from.file ? 1 : -1;
    pieces[move.color][Piece::KING] = 0;
    pieces[move.color][Piece::KING] = move.to.squareToU64();
    mailbox[sqIdx(from)] = Piece::NUM_PIECES;
    mailbox[sqIdx(to)] = Piece::KING;
    // Derive the rook's from and to squares on this side's back rank.
    Bitboard rank = move.color == Color::WHITE ? RANK_1 : RANK_8;
    std::uint8_t rankNum = move.color == Color::WHITE ? 0 : 7;
    if (dir == 1) {
      // Kingside.
      pieces[move.color][Piece::ROOK] &= ~(rank & FILE_H);
      pieces[move.color][Piece::ROOK] |= (rank & FILE_F);
      mailbox[rankNum * 8 + 7] = Piece::NUM_PIECES;
      mailbox[rankNum * 8 + 5] = Piece::ROOK;
      zobristHash ^=
          ZobristHash::getPieceHash(Piece::ROOK, move.color, {rankNum, 7});
      zobristHash ^=
          ZobristHash::getPieceHash(Piece::ROOK, move.color, {rankNum, 5});

    } else {
      // Queenside.
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
    // En passant: move our pawn, then remove the captured pawn, which sits on
    // the same rank as `from` and the same file as `to`.
    pieces[move.color][move.piece] &= ~move.from.squareToU64();
    pieces[move.color][move.piece] |= move.to.squareToU64();
    mailbox[sqIdx(from)] = Piece::NUM_PIECES;
    mailbox[sqIdx(to)] = Piece::PAWN;
    Square capturedPawn = {move.from.rank, move.to.file};
    Color other = move.color == Color::WHITE ? Color::BLACK : Color::WHITE;
    pieces[other][Piece::PAWN] &= ~capturedPawn.squareToU64();
    mailbox[sqIdx(capturedPawn)] = Piece::NUM_PIECES;

    // Update the hash for both pawns.
    zobristHash ^= ZobristHash::getPieceHash(Piece::PAWN, other, capturedPawn);
    zobristHash ^= ZobristHash::getPieceHash(move.piece, move.color, to);
  } else if (move.isPawnDoublePush) {
    // Double push: move the pawn and record the en passant target behind it.
    pieces[move.color][move.piece] &= ~move.from.squareToU64();
    pieces[move.color][move.piece] |= move.to.squareToU64();
    mailbox[sqIdx(from)] = Piece::NUM_PIECES;
    mailbox[sqIdx(to)] = Piece::PAWN;
    // The pawn's old square was already XORed out of the hash above.
    int dir = move.color == Color::WHITE ? 1 : -1;
    Square newEnPassant = {static_cast<std::uint8_t>(move.from.rank + dir),
                           move.from.file};
    gameState.enPassant = newEnPassant;
    zobristHash ^= ZobristHash::getEnPassantHash(newEnPassant);
    zobristHash ^= ZobristHash::getPieceHash(move.piece, move.color, to);
  } else if (move.promotion.has_value()) {
    // Promotion (checked before captures, since a promotion can also capture).
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
    // Capture: move our piece first, then clear the captured piece.
    pieces[move.color][move.piece] &= ~move.from.squareToU64();
    pieces[move.color][move.piece] |= move.to.squareToU64();
    mailbox[sqIdx(from)] = Piece::NUM_PIECES;
    mailbox[sqIdx(to)] = Piece::NUM_PIECES;
    mailbox[sqIdx(to)] = move.piece;
    zobristHash ^= ZobristHash::getPieceHash(move.piece, move.color, to);

    // Remove the captured piece and its hash.
    Color other = move.color == Color::WHITE ? Color::BLACK : Color::WHITE;
    pieces[other][*move.captured] &= ~move.to.squareToU64();
    zobristHash ^= ZobristHash::getPieceHash(*move.captured, other, move.to);
  } else {
    // Quiet move: only our piece moves.
    pieces[move.color][move.piece] &= ~move.from.squareToU64();
    pieces[move.color][move.piece] |= move.to.squareToU64();
    mailbox[sqIdx(from)] = Piece::NUM_PIECES;
    mailbox[sqIdx(to)] = move.piece;
    zobristHash ^= ZobristHash::getPieceHash(move.piece, move.color, to);
  }

  // Castling rights are lost when the king or a corner rook moves, or when a
  // corner rook is captured.
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

// Restores the position from the most recent memento, undoing the hash, game
// state, bitboard and mailbox changes made by makeMove().
void HerodotusEngine::undoMove(void) {
  assert(!history.empty() && "undoMove called with empty history");
  BoardMemento memento = std::move(history.top());
  history.pop();
  auto sqIdx = [](Square s) { return s.rank * 8 + s.file; };
  boardStateHashCount[zobristHash]--;

  const Move &move = memento.move;

  // Remove the current state from the hash.
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

  // Re-add the en passant hash for the restored (pre-move) position.
  if (gameState.enPassant.has_value()) {
    zobristHash ^= ZobristHash::getEnPassantHash(*gameState.enPassant);
  }

  if (move.isCastling) {
    // Castling: move the king back, then the rook.
    pieces[move.color][Piece::KING] = 0;
    pieces[move.color][Piece::KING] = move.from.squareToU64();

    mailbox[sqIdx(move.to)] = Piece::NUM_PIECES;
    mailbox[sqIdx(move.from)] = Piece::KING;

    Bitboard rank = move.color == Color::WHITE ? RANK_1 : RANK_8;
    std::uint8_t rankNum = move.color == Color::WHITE ? 0 : 7;
    int dir = move.to.file > move.from.file ? 1 : -1;

    if (dir == 1) {
      // Kingside.
      pieces[move.color][Piece::ROOK] &= ~(rank & FILE_F);
      pieces[move.color][Piece::ROOK] |= (rank & FILE_H);
      mailbox[rankNum * 8 + 5] = Piece::NUM_PIECES;
      mailbox[rankNum * 8 + 7] = Piece::ROOK;
      zobristHash ^=
          ZobristHash::getPieceHash(Piece::ROOK, move.color, {rankNum, 5});
      zobristHash ^=
          ZobristHash::getPieceHash(Piece::ROOK, move.color, {rankNum, 7});
    } else {
      // Queenside.
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
    // En passant: put our pawn back and restore the captured pawn.
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
    // Double push: put the pawn back; the en passant target came from the
    // memento.
    pieces[move.color][move.piece] &= ~move.to.squareToU64();
    pieces[move.color][move.piece] |= move.from.squareToU64();
    mailbox[sqIdx(move.from)] = Piece::PAWN;
    mailbox[sqIdx(move.to)] = Piece::NUM_PIECES;
    zobristHash ^= ZobristHash::getPieceHash(move.piece, move.color, move.from);
  } else if (move.promotion.has_value()) {
    // Promotion: restore the pawn and remove the promoted piece.
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
    // Capture: put our piece back, then restore the captured piece.
    pieces[move.color][move.piece] |= move.from.squareToU64();
    pieces[move.color][move.piece] &= ~move.to.squareToU64();
    mailbox[sqIdx(move.from)] = move.piece;
    mailbox[sqIdx(move.to)] = *move.captured;
    zobristHash ^= ZobristHash::getPieceHash(move.piece, move.color, move.from);

    // Restore the captured piece and its hash.
    Color other = move.color == Color::WHITE ? Color::BLACK : Color::WHITE;
    pieces[other][*move.captured] |= move.to.squareToU64();
    zobristHash ^= ZobristHash::getPieceHash(*move.captured, other, move.to);
  } else {
    // Quiet move: put our piece back.
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

void HerodotusEngine::makeMoveFast(Move move) {
  // Getting info
  Square from = move.from;
  Square to = move.to;
  auto sqIdx = [](Square s) { return s.rank * 8 + s.file; };
  Piece movingPiece = mailbox[sqIdx(from)];
  if (move.isCastling) {
    // Castling: the king moves two squares and the rook hops over it.
    int dir = move.to.file > move.from.file ? 1 : -1;
    pieces[move.color][Piece::KING] = 0;
    pieces[move.color][Piece::KING] = move.to.squareToU64();
    mailbox[sqIdx(from)] = Piece::NUM_PIECES;
    mailbox[sqIdx(to)] = Piece::KING;
    // Derive the rook's from and to squares on this side's back rank.
    Bitboard rank = move.color == Color::WHITE ? RANK_1 : RANK_8;
    std::uint8_t rankNum = move.color == Color::WHITE ? 0 : 7;
    if (dir == 1) {
      // Kingside.
      pieces[move.color][Piece::ROOK] &= ~(rank & FILE_H);
      pieces[move.color][Piece::ROOK] |= (rank & FILE_F);
      mailbox[rankNum * 8 + 7] = Piece::NUM_PIECES;
      mailbox[rankNum * 8 + 5] = Piece::ROOK;
    } else {
      // Queenside.
      pieces[move.color][Piece::ROOK] &= ~(rank & FILE_A);
      pieces[move.color][Piece::ROOK] |= (rank & FILE_D);
      mailbox[rankNum * 8 + 0] = Piece::NUM_PIECES;
      mailbox[rankNum * 8 + 3] = Piece::ROOK;
    }
  } else if (move.isEnPassant) {
    // En passant: move our pawn, then remove the captured pawn, which sits on
    // the same rank as `from` and the same file as `to`.
    pieces[move.color][move.piece] &= ~move.from.squareToU64();
    pieces[move.color][move.piece] |= move.to.squareToU64();
    mailbox[sqIdx(from)] = Piece::NUM_PIECES;
    mailbox[sqIdx(to)] = Piece::PAWN;
    Square capturedPawn = {move.from.rank, move.to.file};
    Color other = move.color == Color::WHITE ? Color::BLACK : Color::WHITE;
    pieces[other][Piece::PAWN] &= ~capturedPawn.squareToU64();
    mailbox[sqIdx(capturedPawn)] = Piece::NUM_PIECES;
  } else if (move.isPawnDoublePush) {
    // Double push: move the pawn and record the en passant target behind it.
    pieces[move.color][move.piece] &= ~move.from.squareToU64();
    pieces[move.color][move.piece] |= move.to.squareToU64();
    mailbox[sqIdx(from)] = Piece::NUM_PIECES;
    mailbox[sqIdx(to)] = Piece::PAWN;
  } else if (move.promotion.has_value()) {
    // Promotion (checked before captures, since a promotion can also capture).
    pieces[move.color][move.piece] &= ~move.from.squareToU64();
    pieces[move.color][*move.promotion] |= move.to.squareToU64();
    mailbox[sqIdx(from)] = Piece::NUM_PIECES;
    mailbox[sqIdx(to)] = Piece::NUM_PIECES;
    mailbox[sqIdx(to)] = *move.promotion;
    if (move.captured.has_value()) {
      Color other = move.color == Color::WHITE ? Color::BLACK : Color::WHITE;
      pieces[other][*move.captured] &= ~move.to.squareToU64();
    }
  } else if (move.captured.has_value()) {
    // Capture: move our piece first, then clear the captured piece.
    pieces[move.color][move.piece] &= ~move.from.squareToU64();
    pieces[move.color][move.piece] |= move.to.squareToU64();
    mailbox[sqIdx(from)] = Piece::NUM_PIECES;
    mailbox[sqIdx(to)] = Piece::NUM_PIECES;
    mailbox[sqIdx(to)] = move.piece;
    // Remove the captured piece and its hash.
    Color other = move.color == Color::WHITE ? Color::BLACK : Color::WHITE;
    pieces[other][*move.captured] &= ~move.to.squareToU64();
  } else {
    // Quiet move: only our piece moves.
    pieces[move.color][move.piece] &= ~move.from.squareToU64();
    pieces[move.color][move.piece] |= move.to.squareToU64();
    mailbox[sqIdx(from)] = Piece::NUM_PIECES;
    mailbox[sqIdx(to)] = move.piece;
  }
  // done, that should be it, to make a move, we shall test if we can remove
  // mailbox operations next
}

void HerodotusEngine::undoMoveFast(Move move) {
  auto sqIdx = [](Square s) { return s.rank * 8 + s.file; };
  if (move.isCastling) {
    // Castling: move the king back, then the rook.
    pieces[move.color][Piece::KING] = 0;
    pieces[move.color][Piece::KING] = move.from.squareToU64();

    mailbox[sqIdx(move.to)] = Piece::NUM_PIECES;
    mailbox[sqIdx(move.from)] = Piece::KING;

    Bitboard rank = move.color == Color::WHITE ? RANK_1 : RANK_8;
    std::uint8_t rankNum = move.color == Color::WHITE ? 0 : 7;
    int dir = move.to.file > move.from.file ? 1 : -1;

    if (dir == 1) {
      // Kingside.
      pieces[move.color][Piece::ROOK] &= ~(rank & FILE_F);
      pieces[move.color][Piece::ROOK] |= (rank & FILE_H);
      mailbox[rankNum * 8 + 5] = Piece::NUM_PIECES;
      mailbox[rankNum * 8 + 7] = Piece::ROOK;
    } else {
      // Queenside.
      pieces[move.color][Piece::ROOK] &= ~(rank & FILE_D);
      pieces[move.color][Piece::ROOK] |= (rank & FILE_A);
      mailbox[rankNum * 8 + 3] = Piece::NUM_PIECES;
      mailbox[rankNum * 8 + 0] = Piece::ROOK;
    }
  } else if (move.isEnPassant) {
    // En passant: put our pawn back and restore the captured pawn.
    pieces[move.color][move.piece] &= ~move.to.squareToU64();
    pieces[move.color][move.piece] |= move.from.squareToU64();
    mailbox[sqIdx(move.to)] = Piece::NUM_PIECES;
    mailbox[sqIdx(move.from)] = Piece::PAWN;

    Square capturedPawn = {move.from.rank, move.to.file};
    Color other = move.color == Color::WHITE ? Color::BLACK : Color::WHITE;
    pieces[other][Piece::PAWN] |= capturedPawn.squareToU64();
    mailbox[sqIdx(capturedPawn)] = Piece::PAWN;
  } else if (move.isPawnDoublePush) {
    // Double push: put the pawn back; the en passant target came from the
    // memento.
    pieces[move.color][move.piece] &= ~move.to.squareToU64();
    pieces[move.color][move.piece] |= move.from.squareToU64();
    mailbox[sqIdx(move.from)] = Piece::PAWN;
    mailbox[sqIdx(move.to)] = Piece::NUM_PIECES;
  } else if (move.promotion.has_value()) {
    // Promotion: restore the pawn and remove the promoted piece.
    pieces[move.color][move.piece] |= move.from.squareToU64();
    pieces[move.color][*move.promotion] &= ~move.to.squareToU64();
    mailbox[sqIdx(move.from)] = Piece::PAWN;
    mailbox[sqIdx(move.to)] = Piece::NUM_PIECES;
    if (move.captured.has_value()) {
      Color other = move.color == Color::WHITE ? Color::BLACK : Color::WHITE;
      pieces[other][*move.captured] |= move.to.squareToU64();
      mailbox[sqIdx(move.to)] = *move.captured;
    }
  } else if (move.captured.has_value()) {
    // Capture: put our piece back, then restore the captured piece.
    pieces[move.color][move.piece] |= move.from.squareToU64();
    pieces[move.color][move.piece] &= ~move.to.squareToU64();
    mailbox[sqIdx(move.from)] = move.piece;
    mailbox[sqIdx(move.to)] = *move.captured;

    // Restore the captured piece and its hash.
    Color other = move.color == Color::WHITE ? Color::BLACK : Color::WHITE;
    pieces[other][*move.captured] |= move.to.squareToU64();
  } else {
    // Quiet move: put our piece back.
    pieces[move.color][move.piece] |= move.from.squareToU64();
    pieces[move.color][move.piece] &= ~move.to.squareToU64();
    mailbox[sqIdx(move.from)] = move.piece;
    mailbox[sqIdx(move.to)] = Piece::NUM_PIECES;
  }
}

// Generates pseudo-legal moves, then filters out the ones that leave the king
// in check.
std::vector<Move> HerodotusEngine::generateAllLegalMoves() {
  std::vector<Move> pseudoLegal;
  MoveGen::generatePseudoLegalMoves(*this, pseudoLegal);
  return MoveGen::pseudoToLegalMoves(*this, pseudoLegal);
}
