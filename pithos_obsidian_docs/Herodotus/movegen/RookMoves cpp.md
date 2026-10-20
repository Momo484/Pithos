---
module: Herodotus
section: movegen
source: src/engines/herodotus/movegen/RookMoves.cpp
status: documented
---

# RookMoves.cpp

## Overview

This file contains the function implementation for generating rook moves using magic bitboard hashing to compute rank and file attacks, then splitting them into captures and quiet moves. Structurally it is the same as [[BishopMoves cpp]], just pointing at the rook attack tables.

## Structure

### `generateRookMoves`

```cpp
void generateRookMoves(const PositionBitboards &pieces, const Mailbox &mailbox,
                       Bitboard friendly, Bitboard enemy, Color side,
                       std::vector<Move> &out);
```

Given the bitboards, mailbox and an output vector, this function generates every pseudo-legal rook move for the given side. It does so by looping over each set bit in the side's rook bitboard, and for each rook:

1. Build `occupancy = friendly | enemy` — the full board, since a slider cares about every blocker, not just the enemies.
2. Ask `MagicBitboards::getRookAttacks(index, occupancy)` for the attacks along the four straight directions, each ray stopping at the first blocker (inclusive, so blockers are capturable).
3. Mask out friendly pieces with `&= ~friendly`.
4. Split into captures (`& enemy`) and quiet moves (`& ~enemy`), pushing captures with `mailbox[captureIdx]` and quiet moves with `std::nullopt`.

## Design notes

- Same situation as bishops: a rook's attacks depend on the blockers in front of it, so it goes through the magic bitboard tables from [[MagicBitboards cpp]] rather than a fixed table. The occupancy is hashed and used as an index into the precomputed rook attack table.
- `occupancy` is assembled here from `friendly | enemy` rather than passed in as an all-pieces bitboard. It is the same value, but keeping the parameters as they are means this file has no dependency on the engine class.
- Same as the other piece generators, the mailbox is only used on captures for an O(1) captured piece lookup.
