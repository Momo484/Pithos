---
module: Herodotus
section: movegen
source: src/engines/herodotus/movegen/QueenMoves.hpp
status: documented
---

# QueenMoves.hpp

## Overview

This file contains the function prototype for generating all possible queen moves on the chessboard. Queens combine the movement patterns of both bishops and rooks.

## Structure

### `generateQueenMoves`

```cpp
void generateQueenMoves(
    const PositionBitboards &pieces,      // Bitboard representation of all pieces on the board
    const Mailbox &mailbox,                // Stores piece information per square (for captured piece)
    Bitboard friendly,                     // Bitboard of friendly pieces (excluded from moves)
    Bitboard enemy,                        // Bitboard of enemy pieces (captures allowed here)
    Color side,                            // Which color's queen moves to generate
    std::vector<Move> &out                 // Output vector to store generated moves
);
```

**Purpose:** Iterates through all queens of the specified color and generates all legal moves by combining diagonal attacks (like bishops) and straight-line attacks along ranks and files (like rooks), then filtering out friendly pieces and capturing enemy pieces.

See [[QueenMoves cpp]] for implementation details.

## Design notes

- The mailbox is in the signature for captures — `mailbox[captureIdx]` gives the captured piece type in O(1) rather than scanning the enemy bitboards. Everything else is passed as plain bitboards, so this file has no dependency on the engine class.
- The queen has no attack table of her own. She reuses the magic bitboard tables from [[MagicBitboards hpp]] by OR'ing the rook and bishop results together, which is exactly what her movement is.
