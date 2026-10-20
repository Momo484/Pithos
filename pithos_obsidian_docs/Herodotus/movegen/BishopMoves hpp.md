---
module: Herodotus
section: movegen
source: src/engines/herodotus/movegen/BishopMoves.hpp
status: documented
---

# BishopMoves.hpp

## Overview

This file contains the function prototype for generating all possible bishop moves on the chessboard using magic bitboard hashing to efficiently compute diagonal attacks.

## Structure

### `generateBishopMoves`

```cpp
void generateBishopMoves(
    const PositionBitboards &pieces,      // Bitboard representation of all pieces on the board
    const Mailbox &mailbox,                // Stores piece information per square (for captured piece)
    Bitboard friendly,                     // Bitboard of friendly pieces (excluded from moves)
    Bitboard enemy,                        // Bitboard of enemy pieces (captures allowed here)
    Color side,                            // Which color's bishop moves to generate
    std::vector<Move> &out                 // Output vector to store generated moves
);
```

**Purpose:** Iterates through all bishops of the specified color and generates all legal moves by using magic bitboard hashing to compute diagonal attacks, then filtering out friendly pieces and capturing enemy pieces.

See [[BishopMoves cpp]] for implementation details.
