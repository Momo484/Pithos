---
module: Herodotus
section: movegen
source: src/engines/herodotus/movegen/KnightMoves.cpp
status: documented
---

# KnightMoves.cpp

## Overview

This file contains the function implementation for generating knight moves, which is just a matter of looking up the precomputed attack table for each knight and splitting the results into captures and quiet moves.

## Structure

### `generateKnightMoves`

```cpp
void generateKnightMoves(const PositionBitboards &pieces,
                         const Mailbox &mailbox, Bitboard friendly,
                         Bitboard enemy, Color side, std::vector<Move> &out);
```

Given the bitboards, mailbox and an output vector, this function generates every pseudo-legal knight move for the given side. It does so by looping over each set bit in the side's knight bitboard (grabbing the lowest set bit with `__builtin_ctzll`, then clearing it), and for each knight:

1. Look up `BitboardTables::knightAttacks[index]` to get every square the knight can attack.
2. Mask out friendly pieces with `&= ~friendly` so we never generate a move onto our own piece.
3. Split the remainder into captures (`& enemy`) and quiet moves (`& ~enemy`).
4. Push each capture with `mailbox[captureIdx]` as the captured piece, and each quiet move with `std::nullopt` for both `captured` and `promotion`.

## Design notes

- The attack table is constexpr, so it is generated at compile time and costs nothing at runtime — a single array index replaces what would otherwise be eight conditional shifts with file masks.
- The mailbox is only ever read on captures. It gives the captured piece type in O(1), which is needed to fully describe the move without having to work it out again later during make move.
- The capture/quiet split means quiet moves never touch the mailbox at all, so empty squares cost nothing.
- Nothing here deals with legality — that is all handled downstream in [[MoveGen cpp]].
