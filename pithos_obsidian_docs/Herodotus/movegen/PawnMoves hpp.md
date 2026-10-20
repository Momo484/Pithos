---
module: Herodotus
section: movegen
source: src/engines/herodotus/movegen/PawnMoves.hpp
status: documented
---

# PawnMoves.hpp

## Overview

This file contains the function prototype for generating all possible pawn moves on the chessboard, including single pushes, double pushes, captures, promotions, and en passant.

## Structure

### `generatePawnMoves`

```cpp
void generatePawnMoves(
    const PositionBitboards &pieces,      // Bitboard representation of all pieces on the board
    const Mailbox &mailbox,                // Stores piece information per square (for captured piece)
    Bitboard friendly,                     // Bitboard of friendly pieces (excluded from moves)
    Bitboard enemy,                        // Bitboard of enemy pieces (captures allowed here)
    Bitboard all,                          // Bitboard of all pieces (used for empty square checks)
    Color side,                            // Which color's pawns to generate moves for
    const GameState &state,                // Current game state (en passant square, castling rights)
    std::vector<Move> &out                 // Output vector to store generated moves
);
```

**Purpose:** Generates all legal pawn moves including:
- Single forward pushes (one square)
- Double forward pushes from starting position (two squares)
- Captures on diagonals (left and right)
- Promotions when reaching the opposite end of the board
- En passant captures when applicable

See [[PawnMoves cpp]] for implementation details.

## Design notes

- The mailbox is in the signature for captures — `mailbox[to]` gives the captured piece type in O(1) rather than scanning the enemy bitboards. Everything else is passed as plain bitboards, so this file has no dependency on the engine class.
- Pawns are the one piece with no attack table. Their movement is simple enough (and direction-dependent on colour) that raw shifts with file masks do the job, so nothing is precomputed here.
- The `all` parameter exists purely for the empty-square checks on single and double pushes — the other piece generators don't need it since their attack tables already account for blockers.
