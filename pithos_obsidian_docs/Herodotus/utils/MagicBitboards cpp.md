---
module: Herodotus
section: utils
source: src/engines/herodotus/utils/MagicBitboards.cpp
status: stub
---
# MagicBitboards cpp
## Overview
Contained here are all of the runtime based functions and runtime filled tables, unlike in the hpp where tables and functions were largely compile time based. This is where the attack tables and their respective getters are found, and used during move generation.
## Structure
**std::array<std::array<Bitboard, \_Table_SIZE>, Square::NUM_SQUARES> \_Attacks**
- These are the two attack tables, filled in by initMagicBitboards at launch. For each square, they contain the possible attacks dependent on any combination of relevant blockers, the index for the correct attack is found by using the magic numbers and appropriate right bit shifts.
**Bitboard generate\_Attacks**
- given a start square and blockers, these function generate the possible attack/moves for a bishop or rook dependent on the variation of the function. It uses the rayCast function defined in the hpp.
**uint64_t findMagic**
- This function finds a valid magic, given a square, relevant squares mask, attack generation function and random number generator. It does so by first generating all the possible subsets of the relevant squares mask using the carry rippler, then it generates all of the possible attacks for each subset using the generateAttacks function passed in, which would be one of the two noted above. Finally, it generates random numbers, and attempts to fill a table without clashes. To minimise table allocation we use a stamp + epoch method, so each table index is given an epoch when used, and the stamp notes the current epoch.
**uiny64_t find\_Magic**
- given a square, this function places the correct parameters for finding a magic number for the square, for either a rook or a bishop.
**void initMagicBitboards**
- This function is called at startup to fill out the attack tables in such a way that the magic numbers and shifts correctly map to the correct attacks. it does so by using carry ripler to generate every possible relevant blocker combination, for each, generating the attack, then mapping it to the index retrieved by (occ * magic) >> shift.
**Bitboard getRook/Bishop/QueenAttacks**
- Given an index (square) and occupancy bitboard, this indexes into the attack tables to find the attack bitboard for the move generator. QueenAttacks simply or's both rook and bishop results.
## Design notes
- I chose to add getters for some reason. It was leftovers from AI first coded this, and so would have required lots of changes across the rest of move generation, so i left it like this in my rewrite.
- The attack tables could have been designed to be flat arrays for better cache handling, but I coudln't be bothered, i might come back to this later. 