---
module: Herodotus
section: movegen
source: src/engines/herodotus/movegen/KingMoves.hpp
status: documented
---

# KingMoves.hpp

## Overview

This file contains the function prototype for generating all possible king moves on the chessboard, including normal moves, captures, and castling rights.

## Structure

### `generateKingMoves`

```cpp
void generateKingMoves(
    const PositionBitboards &pieces,      // Bitboard representation of all pieces on the board
    const Mailbox &mailbox,                // Stores piece information per square (for captured piece)
    Bitboard friendly,                     // Bitboard of friendly pieces (excluded from moves)
    Bitboard enemy,                        // Bitboard of enemy pieces (captures allowed here)
    Bitboard all,                          // Bitboard of all pieces (used to check king safety for castling)
    Color side,                            // Which color's king moves to generate
    const GameState &state,                // Current game state (castling rights, en passant info)
    std::vector<Move> &out                 // Output vector to store generated moves
);
```

**Purpose:** Generates all legal king moves including:
- Normal one-square moves in any direction
- Captures of adjacent enemy pieces
- Castling moves (both kingside and queenside) when castling rights are available and paths are clear

See [[KingMoves cpp]] for implementation details.
