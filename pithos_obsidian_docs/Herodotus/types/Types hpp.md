---
module: Herodotus
section: types
source: src/engines/herodotus/types/Types.hpp
status: documented
---

# Types.hpp

## Overview

This file contains the core definitions and enumerations for the Herodotus chess engine: the `Piece` and `Color` enums, plus the container aliases that make up the board representation vocabulary.

## Structure

### Piece enum

```cpp
enum Piece { PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, NUM_PIECES };
```

The 6 possible piece types, plus `NUM_PIECES` which keeps the count (0-indexed: pawn, knight, bishop, rook, queen, king, num_pieces). I created this enum so that code indexing into bitboard arrays leaves no room for confusion.

### Color enum

```cpp
enum Color { WHITE, BLACK, NUM_COLORS };
```

American spelling, I know — it seems to be standard in the space, so I stuck with it. Same reason as the `Piece` enum, and the two are often used hand in hand. The order is: white, black, num_colors (0-indexed).

### Type aliases

```cpp
using PieceBitboards = std::array<Bitboard, NUM_PIECES>;
using PositionBitboards = std::array<PieceBitboards, NUM_COLORS>;
using Mailbox = std::array<Piece, 64>;
```

- `PieceBitboards` — An array of bitboards of size 6 (`NUM_PIECES`), one per piece type. I chose an array over a vector because the sizing is static, which I believe lends itself to lower memory overhead and performance gains.
- `PositionBitboards` — An array of `PieceBitboards` of size `NUM_COLORS`, i.e., all piece bitboards for each colour. Same array-over-vector reasoning.
- `Mailbox` — An array mapping a square (by index [0, 64]) to a piece (or `NUM_PIECES` if empty). Bitboards alone cannot answer "what piece is on this square", so the mailbox acts as the lookup companion.

## Design notes

The enums double as array indices throughout the engine, so `pieces[color][pieceType]` reads unambiguously thanks to them. Keeping the aliases here as well means the board representation vocabulary is defined in exactly one place.
