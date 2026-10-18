---
module: Herodotus
section: utils
source: src/engines/herodotus/utils/BitboardTables.hpp
status: stub
---
# BitboardTables hpp

## Overview
This file contains the generative code for three helper bitboards.
1. Squares between -> given index or bitboard masks, this bitboard gives a bitboard with the bits directly between the two square masked off. This initially was useful before magic bitboard implementation, for generating rook, bishop and queen attacks. Now it is only used for checking castling rights during king move generation.
2. Knights attacks -> given the index of a knight, this table contains all the possible knight attacks in the form of a bitboard mask, for quick retrieval. It is used in KnightMoves.cpp during move generation.
3. King attacks -> does a similar job to knight attacks, used in KingMoves.cpp.
## Structure
Each of the three helper bitboard are first declared as a constexpr (specifies that the value of a variable can be evaluated at compile time, this implies const (value does not change) and inline (not quite sure yet)), the function to generate the bitboard follows, then finally the actual bitboard is declared as an inline constexpr.

## Design notes
Generating the bitboard at compile time ensures that unnecessary time is not spent at startup generating these bitboards, but also we do not have to hard code each possible combination for 64^2 possibilities. Furthermore, these expressions are written and declared in the hpp file directly because the compiler needs to see their full definition at compile time, rather than being hidden in the cpp file.

**Todo/Note**
Also, i feel that there is some redundancy between these helpers and some of the inline helper found in [[MagicBitboards hpp]], as square between are not used as they should be.