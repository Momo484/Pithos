---
module: Herodotus
section: utils
source: src/engines/herodotus/utils/MagicBitboards.cpp
status: documented
---

# MagicBitboards.cpp

## Overview

Contained here are all of the runtime-based functions and runtime-filled tables, unlike in the hpp where tables and functions were largely compile-time based. This is where the attack tables and their respective getters are found, and used during move generation.

## Structure

```cpp
std::array<std::array<Bitboard, _Table_SIZE>, Square::NUM_SQUARES> _Attacks[];
```

These are the two attack tables (one for rooks, one for bishops), filled in by `initMagicBitboards` at launch. For each square, they contain the possible attacks dependent on any combination of relevant blockers. The index for the correct attack is found by using the magic numbers and appropriate right bit shifts.

```cpp
generate_Attacks(Square start, Bitboard blockers);
```

Given a start square and blockers, this function generates the possible attack/moves for a bishop or rook dependent on the variation of the function. It uses the `rayCast` function defined in the hpp.

```cpp
findMagic(Square square, std::function<Bitboard(Square, Bitboard)> generateAttacks, Random& rng);
```

This function finds a valid magic, given a square, relevant squares mask, attack generation function and random number generator. It does so by first generating all the possible subsets of the relevant squares mask using the carry rippler, then it generates all of the possible attacks for each subset using the `generateAttacks` function passed in, which would be one of the two noted above. Finally, it generates random numbers and attempts to fill a table without clashes. To minimise table allocation we use a stamp + epoch method, so each table index is given an epoch when used, and the stamp notes the current epoch.

```cpp
uint64_t find_Magic(Square square, Piece pieceType);
```

Given a square, this function places the correct parameters for finding a magic number for the square, for either a rook or a bishop.

```cpp
initMagicBitboards();
```

This function is called at startup to fill out the attack tables in such a way that the magic numbers and shifts correctly map to the correct attacks. It does so by using carry rippler to generate every possible relevant blocker combination, for each generating the attack, then mapping it to the index retrieved by `(occ * magic) >> shift`.

```cpp
getRookAttacks(int index, Bitboard occupancy);
getBishopAttacks(int index, Bitboard occupancy);
getQueenAttacks(int index, Bitboard occupancy);  // Simply OR's both rook and bishop results
```

Given an index (square) and occupancy bitboard, this indexes into the attack tables to find the attack bitboard for the move generator. `QueenAttacks` simply OR's both rook and bishop results.

## Design notes

- I chose to add getters for some reason. It was leftovers from AI first coded this, and so would have required lots of changes across the rest of move generation, so I left it like this in my rewrite.
- The attack tables could have been designed to be flat arrays for better cache handling, but I couldn't be bothered, I might come back to this later.
