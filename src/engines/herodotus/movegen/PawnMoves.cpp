#include "movegen/PawnMoves.hpp"
#include "types/Move.hpp"
#include "types/Square.hpp"

namespace MoveGen {

// Generates every pseudo-legal pawn move for `side`: pushes, captures,
// promotions and en passant. The logic is written out twice, once per colour,
// since pawn direction depends on which colour is moving.
void generatePawnMoves(const PositionBitboards &pieces, const Mailbox &mailbox,
                       Bitboard friendly, Bitboard enemy, Bitboard all,
                       Color side, const GameState &state,
                       std::vector<Move> &out) {

  bool isWhite = side == Color::WHITE;
  Bitboard empty = ~all;

  // Shared lambda for anything landing on a square: reads the captured piece
  // off the mailbox and pushes the move, expanding to all four promotions when
  // the destination is on the promotion rank.
  int promoRank = isWhite ? 7 : 0;
  auto pushCapture = [&](int toSq, Square from, Square to) {
    Piece captured = mailbox[toSq];
    if (to.rank == promoRank) {
      out.push_back({side, Piece::PAWN, from, to, captured, Piece::KNIGHT,
                     false, false, false});
      out.push_back({side, Piece::PAWN, from, to, captured, Piece::BISHOP,
                     false, false, false});
      out.push_back({side, Piece::PAWN, from, to, captured, Piece::ROOK, false,
                     false, false});
      out.push_back({side, Piece::PAWN, from, to, captured, Piece::QUEEN, false,
                     false, false});
    } else {
      out.push_back({side, Piece::PAWN, from, to, captured, std::nullopt, false,
                     false, false});
    }
  };

  if (isWhite) {
    Bitboard pawnBB = pieces[Color::WHITE][Piece::PAWN];
    // --- Single pushes ---
    Bitboard singlePushes = (pawnBB << 8) & empty;

    // Promotions (reached rank 8)
    Bitboard promotions = singlePushes & RANK_8;
    while (promotions) {
      // Count trailing zeroes to find the index of the next set bit.
      int toSq = __builtin_ctzll(promotions);
      Square to = Square::fromIndex(toSq);
      Square from = {static_cast<std::uint8_t>(to.rank - 1), to.file};
      out.push_back({side, Piece::PAWN, from, to, std::nullopt, Piece::KNIGHT,
                     false, false, false});
      out.push_back({side, Piece::PAWN, from, to, std::nullopt, Piece::BISHOP,
                     false, false, false});
      out.push_back({side, Piece::PAWN, from, to, std::nullopt, Piece::ROOK,
                     false, false, false});
      out.push_back({side, Piece::PAWN, from, to, std::nullopt, Piece::QUEEN,
                     false, false, false});
      // Clears the lowest set bit (and everything below it); & with the
      // original keeps only the bits above it. Neat trick.
      promotions &= promotions - 1;
    }

    // Non-promotion single pushes
    // Masking off the promotion rank lets us ignore promotions here.
    Bitboard nonPromo = singlePushes & ~RANK_8;
    while (nonPromo) {
      int toSq = __builtin_ctzll(nonPromo);
      Square to = Square::fromIndex(toSq);
      Square from = {static_cast<std::uint8_t>(to.rank - 1), to.file};
      out.push_back({side, Piece::PAWN, from, to, std::nullopt, std::nullopt,
                     false, false, false});
      nonPromo &= nonPromo - 1;
    }

    // --- Double pushes ---
    // Pawns that single-pushed to rank 3 can push again to rank 4
    Bitboard doublePushes = ((singlePushes & RANK_3) << 8) & empty;
    while (doublePushes) {
      int toSq = __builtin_ctzll(doublePushes);
      Square to = Square::fromIndex(toSq);
      Square from = {static_cast<std::uint8_t>(to.rank - 2), to.file};
      out.push_back({side, Piece::PAWN, from, to, std::nullopt, std::nullopt,
                     false, false, true});
      doublePushes &= doublePushes - 1;
    }

    // --- Captures ---

    Bitboard capEast = (pawnBB << 9) & ~FILE_A & enemy;
    while (capEast) {
      int toSq = __builtin_ctzll(capEast);
      Square to = Square::fromIndex(toSq);
      Square from = {static_cast<std::uint8_t>(to.rank - 1),
                     static_cast<std::uint8_t>(to.file - 1)};
      pushCapture(toSq, from, to);
      capEast &= capEast - 1;
    }

    Bitboard capWest = (pawnBB << 7) & ~FILE_H & enemy;
    while (capWest) {
      int toSq = __builtin_ctzll(capWest);
      Square to = Square::fromIndex(toSq);
      Square from = {static_cast<std::uint8_t>(to.rank - 1),
                     static_cast<std::uint8_t>(to.file + 1)};
      pushCapture(toSq, from, to);
      capWest &= capWest - 1;
    }

    // --- En Passant ---

    if (state.enPassant.has_value()) {
      Bitboard epBB = state.enPassant->squareToU64();
      // Pawns one rank below the en passant square, either side of it. The
      // file masks prevent wrap around.
      Bitboard attackers =
          ((epBB >> 9) & ~FILE_H | (epBB >> 7) & ~FILE_A) & pawnBB;
      while (attackers) {
        int fromSq = __builtin_ctzll(attackers);
        Square from = Square::fromIndex(fromSq);
        Square to = *state.enPassant;
        // I believe make move can derive where the captured pawn was, so no
        // need to record it here.
        Move enPassantMove = {side, Piece::PAWN, from,
                              to,   Piece::PAWN, std::nullopt,
                              true, false,       false};
        out.push_back(enPassantMove);
        attackers &= attackers - 1;
      }
    }

  } else {
    // --- Single pushes (black) ---
    Bitboard pawnBB = pieces[Color::BLACK][Piece::PAWN];
    Bitboard singlePushes = (pawnBB >> 8) & empty;

    // Promotions (reached rank 1)
    Bitboard promotions = singlePushes & RANK_1;
    while (promotions) {
      int toSq = __builtin_ctzll(promotions);
      Square to = Square::fromIndex(toSq);
      Square from = {static_cast<std::uint8_t>(to.rank + 1), to.file};
      out.push_back({side, Piece::PAWN, from, to, std::nullopt, Piece::KNIGHT,
                     false, false, false});
      out.push_back({side, Piece::PAWN, from, to, std::nullopt, Piece::BISHOP,
                     false, false, false});
      out.push_back({side, Piece::PAWN, from, to, std::nullopt, Piece::ROOK,
                     false, false, false});
      out.push_back({side, Piece::PAWN, from, to, std::nullopt, Piece::QUEEN,
                     false, false, false});
      promotions &= promotions - 1;
    }

    // Non-promotion single pushes
    Bitboard nonPromo = singlePushes & ~RANK_1;
    while (nonPromo) {
      int toSq = __builtin_ctzll(nonPromo);
      Square to = Square::fromIndex(toSq);
      Square from = {static_cast<std::uint8_t>(to.rank + 1), to.file};
      out.push_back({side, Piece::PAWN, from, to, std::nullopt, std::nullopt,
                     false, false, false});
      nonPromo &= nonPromo - 1;
    }

    // --- Double pushes (black) ---
    // Pawns that single-pushed to rank 6 can push again to rank 5
    Bitboard doublePushes = ((singlePushes & RANK_6) >> 8) & empty;
    while (doublePushes) {
      int toSq = __builtin_ctzll(doublePushes);
      Square to = Square::fromIndex(toSq);
      Square from = {static_cast<std::uint8_t>(to.rank + 2), to.file};
      out.push_back({side, Piece::PAWN, from, to, std::nullopt, std::nullopt,
                     false, false, true});
      doublePushes &= doublePushes - 1;
    }

    // --- Captures ---
    Bitboard capEast = (pawnBB >> 7) & ~FILE_A & enemy;
    while (capEast) {
      int toSq = __builtin_ctzll(capEast);
      Square to = Square::fromIndex(toSq);
      Square from = {static_cast<std::uint8_t>(to.rank + 1),
                     static_cast<std::uint8_t>(to.file - 1)};
      pushCapture(toSq, from, to);
      capEast &= capEast - 1;
    }
    Bitboard capWest = (pawnBB >> 9) & ~FILE_H & enemy;
    while (capWest) {
      int toSq = __builtin_ctzll(capWest);
      Square to = Square::fromIndex(toSq);
      Square from = {static_cast<std::uint8_t>(to.rank + 1),
                     static_cast<std::uint8_t>(to.file + 1)};
      pushCapture(toSq, from, to);
      capWest &= capWest - 1;
    }
    // --- En Passant ---

    if (state.enPassant.has_value()) {
      Bitboard epBB = state.enPassant->squareToU64();
      // Pawns one rank above the en passant square (black captures downwards),
      // either side of it. The file masks prevent wrap around.
      Bitboard attackers =
          ((epBB << 9) & ~FILE_A | (epBB << 7) & ~FILE_H) & pawnBB;
      while (attackers) {
        int fromSq = __builtin_ctzll(attackers);
        Square from = Square::fromIndex(fromSq);
        Square to = *state.enPassant;
        Move enPassantMove = {side, Piece::PAWN, from,
                              to,   Piece::PAWN, std::nullopt,
                              true, false,       false};
        out.push_back(enPassantMove);
        attackers &= attackers - 1;
      }
    }
  }
}

} // namespace MoveGen
