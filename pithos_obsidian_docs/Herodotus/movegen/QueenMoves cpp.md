---
module: Herodotus
section: movegen
source: src/engines/herodotus/movegen/QueenMoves.cpp
status: documented
---

# QueenMoves.cpp

## Overview

This file contains the function implementation for generating queen moves. The queen is a rook and a bishop combined, and since we already have magic bitboard tables for both, there is no separate queen table — the getter just OR's the two results together.

## Structure

### `generateQueenMoves`

```cpp
void generateQueenMoves(const PositionBitboards &pieces, const Mailbox &mailbox,
                        Bitboard friendly, Bitboard enemy, Color side,
                        std::vector<Move> &out);
```

Given the bitboards, mailbox and an output vector, this function generates every pseudo-legal queen move for the given side. It does so by looping over each set bit in the side's queen bitboard, and for each queen:

1. Build `occupancy = friendly | enemy` — the full board, since a slider cares about every blocker, not just the enemies.
2. Ask `MagicBitboards::getQueenAttacks(index, occupancy)`, which is `getRookAttacks | getBishopAttacks` under the hood.
3. Mask out friendly pieces with `&= ~friendly`.
4. Split into captures (`& enemy`) and quiet moves (`& ~enemy`), pushing captures with `mailbox[captureIdx]` and quiet moves with `std::nullopt`.

## Design notes

- There is no dedicated queen attack table. Because a queen's moves are exactly the union of rook and bishop moves, the getter reuses both existing tables and OR's them. It means a queen lookup does two table lookups instead of one, but it saves an entire set of magic tables and the init time that would come with them.
- The mailbox is only used on captures for an O(1) captured piece lookup, same as the other piece generators.
- You might think the queen generator could just call the rook and bishop generators with a different piece type. It could, but duplicating the loop here keeps each generator simple and avoids passing piece type parameters around.
