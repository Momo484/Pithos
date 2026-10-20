---
module: Herodotus
section: movegen
source: src/engines/herodotus/movegen/KingMoves.cpp
status: documented
---

# KingMoves.cpp

## Overview

This file contains the function implementation for generating king moves: normal one-square moves and captures off the precomputed king attack table, plus castling moves generated pseudo-legally from the castling rights in the game state.

## Structure

### `generateKingMoves`

```cpp
void generateKingMoves(const PositionBitboards &pieces, const Mailbox &mailbox,
                       Bitboard friendly, Bitboard enemy, Bitboard all,
                       Color side, const GameState &state,
                       std::vector<Move> &out);
```

Given the bitboards, mailbox, game state and an output vector, this function generates every pseudo-legal king move for the given side. It does so in three parts:

1. **Normal moves and captures** — take `BitboardTables::kingAttacks[fromSq]` for the king's square (found via `__builtin_ctzll` on the king bitboard), remove any squares occupied by friendly pieces, then split what remains into open moves (`& ~enemy`) and capture moves (`& enemy`). Captures read `mailbox[toSq]` to fill in the captured piece, open moves push `std::nullopt`.
2. **Castling** — for each of the four castling moves, check if the corresponding bit is set in `state.castlingRights`, then use `BitboardTables::betweenSquares` to get the squares between the king and rook and verify they are empty against `all`. If so, push a move to the hardcoded destination square (`{0, 6}` / `{0, 2}` for white, `{7, 6}` / `{7, 2}` for black) with `isCastling` set.
3. The white and black castling blocks are separate, as the squares and rights bits differ per colour.

## Design notes

- Castling is deliberately generated pseudo-legally. This function only checks the rights and that the path is empty — it does not care whether the king is in check or passes through an attacked square. That is left to the engine's `validateMove` in [[MoveGen cpp]], which plays out the transit square to make sure the king doesn't move into or through check. I noted this in the source comments too: the engine will be responsible for legalising it.
- The attack table for the king is constexpr, like the knight's, so the eight directional shifts with their file masks are all done at compile time.
- The mailbox is only used on captures for an O(1) captured piece lookup, same as the other piece generators.
- The code only ever looks at the first bit of the king bitboard with `__builtin_ctzll`, which assumes there is exactly one king on the side to move. That is always true in a legal position, so I didn't bother handling anything else.
