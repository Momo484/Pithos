---
module: Herodotus
section: movegen
source: src/engines/herodotus/movegen/KnightMoves.hpp
status: documented
---

# KnightMoves.hpp

## Overview

This file contains the function prototype for generating all possible knight moves on the chessboard using precomputed attack tables for efficiency.

## Structure

### `generateKnightMoves`

```cpp
void generateKnightMoves(
    const PositionBitboards &pieces,      // Bitboard representation of all pieces on the board
    const Mailbox &mailbox,                // Stores piece information per square (for captured piece)
    Bitboard friendly,                     // Bitboard of friendly pieces (excluded from moves)
    Bitboard enemy,                        // Bitboard of enemy pieces (captures allowed here)
    Color side,                            // Which color's knight moves to generate
    std::vector<Move> &out                 // Output vector to store generated moves
);
```

**Purpose:** Iterates through all knights of the specified color and generates all legal moves (non-captures and captures) by consulting precomputed knight attack tables.

See [[KnightMoves cpp]] for implementation details.
