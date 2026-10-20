---
module: Herodotus
section: utils
source: src/engines/herodotus/utils/MagicBitboards.hpp
status: documented
---

# MagicBitboards.hpp

## Overview

This header file contains the function prototypes, hardcoded magic values (generated once and saved), and constexpr functions and tables used for the creation of magic bitboard attack tables used throughout the rest of the move generation code.

## Structure

```cpp
enum RayDirection { /* north, east, south-west, etc. */ };
```

This enum provides the necessary bitshifts required to move a bit in a direction (north, east, south-west...). Left shifts are represented as positive integers, and right shift is negative.

```cpp
constexpr Bitboard rayCast(Square start, RayDirection dir, Bitboard blockers = 0ULL)
```

This function casts a ray from a start square in the direction (see enum above) specified. It was updated in a later version to accept blockers, in which case it stops after meeting the first blocked bit. Note that it still places a bit on the blocked square, so as to allow for attacks if necessary. `0ULL` acts as no blockers (duh!).

```cpp
constexpr std::array<Bitboard, Square::NumSquares> generate_Relevant<RayDirection>();
```

There are two versions of this generator function, one for bishop (diagonal) and one for rooks (straight). This generates an array that, for each of the 64 squares, ray casts in the directions required by the piece, stopping at the final step. This allows us to have immediate masking of "relevant" squares for attacks. This is also why the final step is not included.

```cpp
constexpr auto _Relevant;
```

This is the actual array declaration. It is constexpr like its generator function to ensure that it is generated during compile time, and does not contribute to the startup costs of the engine. Otherwise it could have been hardcoded — this is simpler and less ugly, but leads to slightly longer compilation times.

```cpp
constexpr std::array<int, Square::NUM_SQUARES> generate_Shifts<RayDirection>();
```

Again there are two versions of this function, for the respective pieces. This generator finds the bit shift required for finding the significant bits after the magic number is applied to the relevant blockers: `((occupancy & rookRelevant[index]) * ROOK_MAGICS[index]) >> rookShifts[index]`, seen in rookShifts. It works by counting the number of ones in the relevant blocker array for each square and returning 64 - that number to determine the right shift amount.

```cpp
constexpr auto _shifts;
```

This is the actual array declaration, constexpr for the same reason as the `_relevant` arrays.

### Function Prototypes

The file also contains the following function prototypes, their explanations can be found in [[MagicBitboards cpp]]:

- `void initMagicBitboards()`
- `Bitboard getRookAttacks(int index, Bitboard occupancy)`
- `Bitboard getBishopAttacks(int index, Bitboard occupancy)`
- `Bitboard getQueenAttacks(int index, Bitboard occupancy)`
- `Bitboard generateRookAttacks(Square start, Bitboard blockers)`
- `Bitboard generateBishopAttacks(Square start, Bitboard blockers)`
- `uint64_t findRookMagic(Square square)`
- `uint64_t findBishopMagic(Square square)`


```cpp
constexpr std::array<Bitboard, Square::NUM_SQUARES> _MAGICS[];
```

These are the hardcoded magic numbers for both bishops and rooks. These magic numbers are generated in `generate_magics.cpp` and were copied in.

## Design notes

- I have used constexpr where possible to shift constant expression generation to compile time rather than runtime for fast launching, as I believe that is what it allows for. I believe it is similar to hardcoding the values, however it is cleaner and easier to understand the logic this way, but it does lead to longer compile time (very minimal).
- I have hardcoded the magic values, as this takes some time to generate, and I didn't want to do this on every execution. Also, these are tested and verified to work as intended.
- I declared the ray direction enum here as it made sense when I was coding. Thinking back, this could have been reused across the codebase and should probably be sent to the types folder — this I may do later.

## Study Notes

https://rhysre.net/fast-chess-move-generation-with-magic-bitboards.html

The general idea is to map a masked blockers bitboard to an attack set.

![Diagram 1](../../pithos_obsidian_docs/Herodotus/Pasted%20image%2020261018204230.png)
![Diagram 2](../../pithos_obsidian_docs/Herodotus/Pasted%20image%2020261018204236.png)

What the magic number does is move the masked blocker numbers to the top 10 bits, and so when we right shift by 54 we get only the top 10 bits, which can then be used as a unique index into our table.

- Something to note: masked blockers can only have a maximum of 10 ones, which makes this possible.
Then, given this key, we index into a preinitialised attack set bitboard.

### Implementation Plan

By this logic our implementation plan is to:

1. **Generate Relevant Masks**
   1. For all 64 squares calculate the blocker mask for rooks and bishops (excluding the outer edges of board)

2. **Implement the core math functions**
   1. The indexing function: `(occupancy * magic) >> (64 - bits)`, bits = 10? (this makes sense to make after finding the magic numbers instead).
   2. An on-the-fly attack generator to teach the tables what the correct attacks are.

3. **Find magic numbers**
   1. Create an array of all possible blocker configurations using relevant masks → carry-ripler
   2. Generate true attack bitboards
   3. Run brute force loop that tries numbers until it finds a magic number that maps all the blocker configurations to unique indices
   4. Generate them once and never again!

4. **Populate attack tables**
   1. Loop through all 64 squares
   2. For each square loop through all 2^N blocker combinations
   3. Use hardcoded magic number to calculate the magic index
   4. Store the pre-calculated attack bitboard onto the array: `_attacks[square][magic_index] = brute_force_attack`.

When I finish 3., I'll run them on main (or a separate executable for cleaner code), then paste them in as hardcoded tables.

So far I have:
- Finished the generate relevant masks functions and assigned their corresponding arrays. In doing so, I have made a direction enum and rayCast function which required changes to many of the Square functions to constexpr.
- Finished the naive move generation functions.
- Created constexpr shifts, for quick lookup of right shift amounts at each square to find the correct index into the attack tables.
- Finished making getters for bishop and rook attacks.
- Finished making `initMagicBitboards`.

Completely finished.
