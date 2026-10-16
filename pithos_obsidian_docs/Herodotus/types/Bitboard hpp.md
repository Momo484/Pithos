---
module: Herodotus
section: types
source: src/engines/herodotus/types/Bitboard.hpp
---

# Bitboard.hpp

## Overview
This file contains the typedef for a bitboard plus the constants and masks used across the engine.

## Structure
- `typedef uint64_t Bitboard` — allows us to call `uint64_t` a `Bitboard` across the engine.
- `EMPTY_BOARD` — as it sounds: no squares set.
- `FULL_BOARD` — as it sounds: all 64 squares set.
- `RANK_1` … `RANK_8` — masks covering one row (8 squares) each.
- `FILE_A` … `FILE_H` — masks covering one column (8 squares) each.

## Bit layout
Bits map to squares in little-endian rank-file order: **bit index = rank × 8 + file**, where rank 0–7 means ranks 1–8 and file 0–7 means files a–h. Concretely:
- Bits 0–7: rank 1 (a1–h1)
- Bits 8–15: rank 2 (a2–h2)
- …
- Bits 56–63: rank 8 (a8–h8)

Within each rank, files run a → h with increasing bit value. Every other part of the engine depends on this convention, so it is worth memorising.

## Design notes
Bitboards are the core of the position representation: whole ranks, files and diagonals become a single integer, and occupancy queries and sliding-piece attacks become bitwise operations instead of loops. The rank and file masks exist because they are the cheapest way to isolate a single row or column for those operations.
