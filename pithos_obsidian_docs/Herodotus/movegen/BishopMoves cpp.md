---
module: Herodotus
section: movegen
source: src/engines/herodotus/movegen/BishopMoves.cpp
status: documented
---

# BishopMoves.cpp

## Overview

This file contains the function implementation for generating bishop moves using magic bitboard hashing to compute diagonal attacks, then splitting them into captures and quiet moves.

## Structure

### `generateBishopMoves`

```cpp
void generateBishopMoves(const PositionBitboards &pieces,
                         const Mailbox &mailbox, Bitboard friendly,
                         Bitboard enemy, Color side, std::vector<Move> &out);
```

Given the bitboards, mailbox and an output vector, this function generates every pseudo-legal bishop move for the given side. It does so by looping over each set bit in the side's bishop bitboard, and for each bishop:

1. Build `occupancy = friendly | enemy` — the full board, since a slider cares about every blocker, not just the enemies.
2. Ask `MagicBitboards::getBishopAttacks(index, occupancy)` for the attacks along the four diagonals, each ray stopping at the first blocker (inclusive, so blockers are capturable).
3. Mask out friendly pieces with `&= ~friendly`.
4. Split into captures (`& enemy`) and quiet moves (`& ~enemy`), pushing captures with `mailbox[captureIdx]` and quiet moves with `std::nullopt`.

## Design notes

- A bishop's attacks depend on where everything else is, so unlike the knight and king there is no fixed attack table to index into. The magic bitboard tables from [[MagicBitboards cpp]] turn the occupancy into a hash and a single array lookup, which is about as fast as it gets without ray casting at runtime.
- `occupancy` is assembled here from `friendly | enemy` rather than passed in as an all-pieces bitboard. It is the same value, but keeping the parameters as they are means this file has no dependency on the engine class.
- Same as the other piece generators, the mailbox is only used on captures for an O(1) captured piece lookup.
