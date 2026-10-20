---
module: Herodotus
section: movegen
source: src/engines/herodotus/movegen/RookMoves.hpp
status: documented
---

# RookMoves.hpp

## Overview

This file contains the function prototype for generating all possible rook moves on the chessboard using magic bitboard hashing to efficiently compute rank and file attacks.

## Structure

### `generateRookMoves`

```cpp
void generateRookMoves(
    const PositionBitboards &pieces,      // Bitboard representation of all pieces on the board
    const Mailbox &mailbox,                // Stores piece information per square (for captured piece)
    Bitboard friendly,                     // Bitboard of friendly pieces (excluded from moves)
    Bitboard enemy,                        // Bitboard of enemy pieces (captures allowed here)
    Color side,                            // Which color's rook moves to generate
    std::vector<Move> &out                 // Output vector to store generated moves
);
```

**Purpose:** Iterates through all rooks of the specified color and generates all legal moves by using magic bitboard hashing to compute attacks along ranks and files, then filtering out friendly pieces and capturing enemy pieces.

See [[RookMoves cpp]] for implementation details.

## Design notes

- The mailbox is in the signature for captures — `mailbox[captureIdx]` gives the captured piece type in O(1) rather than scanning the enemy bitboards. Everything else is passed as plain bitboards, so this file has no dependency on the engine class.
- As a slider, the rook has no fixed attack table — what it can reach depends on the blockers in front of it. Attacks go through the magic bitboard tables in [[MagicBitboards hpp]] instead, indexed by a hash of the occupancy.
