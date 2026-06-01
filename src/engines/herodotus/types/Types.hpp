#pragma once

/**
 * @file Types.hpp
 * @brief Core type definitions and enumerations for the Herodotus chess engine.
 *
 * This file contains all fundamental enumeration types used throughout the engine,
 * providing a centralized location for type definitions to avoid scattering
 * these definitions across multiple headers.
 */

/**
 * @enum Piece
 * @brief Enumeration of chess piece types.
 *
 * Each piece type is represented by a unique value that corresponds to an index
 * in the piece bitboard arrays. The index is used to access piece positions.
 */
enum Piece {
  PAWN = 0,      ///< Pawn piece
  KNIGHT = 1,    ///< Knight piece
  BISHOP = 2,    ///< Bishop piece
  ROOK = 3,      ///< Rook piece
  QUEEN = 4,     ///< Queen piece
  KING = 5,      ///< King piece
  NUM_PIECES = 6 ///< Total number of piece types
};

/**
 * @enum Color
 * @brief Enumeration of player colors.
 *
 * Used to distinguish between white and black pieces, moves, and game state.
 * White is always the starting player.
 */
enum Color {
  WHITE = 0,     ///< White player
  BLACK = 1,     ///< Black player
  NUM_COLORS = 2 ///< Total number of colors
};
