---
module: Herodotus
section: types
source: src/engines/herodotus/types/Square.hpp
status: documented
---

# Square.hpp

## Overview

This file holds the square representation and its utilities. A square is a single location on the 8×8 board identified by rank (row) and file (column), both 0-indexed (rank 0 = rank 1, file 0 = a-file).

## Structure

### Square struct

```cpp
struct Square {
    uint8_t rank;   // row of the square (0–7)
    uint8_t file;   // column of the square (0–7)
};
```

### Methods

- `bool isValid()` — Checks both coordinates are within [0, 7].
- `operator==` / `operator!=` — Coordinate-wise comparison.
- `Bitboard squareToU64()` — Converts the square to its bitboard representation (a single bit set); returns 0 if the square is invalid.
- `static Square fromIndex(int index)` — Converts a linear bit index [0, 63] back to a square.
- `static Square fromIndex(Bitboard bb)` — Convenience overload that takes the square of the lowest set bit (via `__builtin_ctzll`).

### Additional

- `SquareHash` — A hash function so squares can be used in unordered containers.
- `constexpr uint8_t NUM_SQUARES = 64;` — Total number of squares on the board.

## Design notes

### Choice of `uint8_t`

I chose `uint8_t` for the sake of saving memory, though I wasn't sure at the time whether it actually did anything given how C++ pads memory. It does: two `uint8_t` members give `Square` a size of 2 bytes (no padding, since `uint8_t` has alignment 1), whereas two `int`s would make it 8 bytes — and squares get stored in bulk.

### Choice of struct over class

I chose a struct initially because I had no plans for built-in methods (I didn't know you could do them in a struct), but when I realised I needed them I saw no reason to change. The privacy of a variable is not a concern by design and inheritance does not seem likely. In essence, a square is a passive data structure: it has further data that can be extracted from it, but in and of itself, it does not do anything.
