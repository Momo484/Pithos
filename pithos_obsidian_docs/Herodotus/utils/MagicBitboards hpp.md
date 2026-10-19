---
module: Herodotus
section: utils
source: src/engines/herodotus/utils/MagicBitboards.hpp
status: stub
---
# MagicBitboards hpp

## Overview

## Structure

## Design notes

## Study Notes 
https://rhysre.net/fast-chess-move-generation-with-magic-bitboards.html
The general idea is to map a masked blockers bitboard to an attack set 
![[Pasted image 20261018204230.png]]![[Pasted image 20261018204236.png]]
What the magic number does is move the masked blocker numbers to the 10 top bits, and so when we right shift by 54 we get only the top 10 bits, when can then be used as a unique index into our table.
- Something to note, masked blockers can only have a maximum of 10 ones, which makes this possible. 
Then given this key, we index into a preinitialised attack set bitboard. 

By this logic our implementation plan is to
1. Generate Relevant Masks
	1. For all 64 squares calculate the blocker mask for rooks and bishops (excluding the outer edges of board)
2. Implement the core math functions 
	1. the indexing function (occupancy * magic) >> (64 - bits), bits = 10? (this makes sense to make after finding the magic numbers instead).
	2. An on the fly attack generator to teach the tables what the correct attacks are .
3. Find magic numbers 
	1. Create an array of all possible blocker configurations using relevant masks? -> carry-ripler
	2. generate true attack bitboards 
	3. run brute force loop that tries numbers until it finds a magic number that maps all the blocker configurations to unique indicies 
	4. generate them once and never again!
4. Populate attack tables 
	1. loop through all 64 squares 
	2. for each square loop through all 2^N blocker combinations
	3. use hardcocded magic number to calculate the magic index 
	4. store the pre-calculated attack bitboard onto the array \_attacks\[square]\[magic_index] = brute_force_attack.

When i finish 3. I'll run them on main (or a seperate executable for cleaner code), then paste them in as hardcoded tables. 

So far i have 
- Finished the generate relevant masks functions and their assigned their corresponding arrays. In doing so i have made a direction enum and rayCast function which required changes many of the Square functions to constexpr.
- Finished the naive move generation functions