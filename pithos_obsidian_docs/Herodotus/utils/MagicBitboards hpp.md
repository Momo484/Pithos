---
module: Herodotus
section: utils
source: src/engines/herodotus/utils/MagicBitboards.hpp
status: stub
---
# MagicBitboards hpp

## Overview
This header file contains the function prototypes, hardcoded magic values (generated once and saved), and constexpr functions and tables used for the creations of magic bitboard attack tables used throughout the rest of the move generation code.
## Structure
**enum RayDirection**
- This enum provides the neccesary bitshifts required to move a bit in a direction (north, east, south-west ...). left shifts are represented as positive integers, and right shift is negative.
**constexpr bitboard rayCast**
- This function casts a ray from a start square in the direction (see unum above) specified. It was updated in a later version to accept blockers, in which case it stops after meeting the first blocked bit. Note that it still places a bit on the blocked square, so as to allow for attacks if neccesary. 0ULL acts as no blockers (duh!).
**constexpr std::array<Bitboard, Square::NumSquares> generate_Relevant()**
- There are two versions of this generator function, one for bishop (diagonal) and one for rooks (straight).  This generates an array, that for each of the 64 squares, ray casts in the directions required by the piece, stopping at the final step. This allows us to have immediate masking of "relevant" squares for attacks. This is also why the final step is not included.
**constexpr auto  \_Relevant**
- This is the actual array declaration, it is constexpr like its generator function to ensure that it is generated during compile time, and does not contribute to the startup costs of the engine. Otherwise it could have been hardcoded, this is simpler and less ugly, but leads to slightly longer compilation times.
**constexpr std::array<int, Square::NUM_SQUARES> generate_Shifts()**
- Again their are two versions of this function, for the respective pieces. This generator, finds the bit shift required for finding the significant bits after the magic number is applied to the relevant blockers, (((occupancy & rookRelevant[index]) * ROOK_MAGICS[index]) >> rookShifts[index])  seen in rookShifts. It works by counting the number of ones in the relevant blocker array for each square and returning 64- that number to determine the right shift amount.
**constexpr auto  \_shifts** 
- This is the actual array declaration, constexpr for the same reason as the \_relevant arrays.
**Function Prototypes**
The file also contains the following function prototypes, their explanations can be found in [[MagicBitboards cpp]]
- void initMagicBitboards();
- Bitboard getRookAttacks(int index, Bitboard occupancy);
- Bitboard getBishopAttacks(int index, Bitboard occupancy);
- Bitboard getQueenAttacks(int index, Bitboard occupancy);
- Bitboard generateRookAttacks(Square start, Bitboard blockers);
- Bitboard generateBishopAttacks(Square start, Bitboard blockers);
- uint64_t findRookMagic(Square square);
- uint64_t findBishopMagic(Square square);
**constexpr std::array<Bitboard, Square::NUM_SQUARES> \_MAGICS**
- These are the hardcoded magic numbers for both bishops and rooks. These magic numbers are generated in generate_magics.cpp and were copied in. 

## Design notes
- I have used constexpr where possible to shift constant expression generation to compile time rather than runtime for fast launching, as i believe that is what it allows for. I believe it is similar to hardcoding the values, however it is cleaner and easier to understand the logic this way, but it does lead to longer compile time (very minimal).
- I have hardcoded the magic values, as this takes some time to generate, and i didn't want to do this on every execution, also these are tested and verified to work as intended.
- I declared the ray direction enum here as it made sense when i was coding, thinking back, this could have been reused across the codebase and should probably be sent to the types folder, this i may do later.


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
- Created constexpr shifts, for quick lookup of right shift amounts at each square to find the correct index in to the attack tables.
- finished making getter for bishop and rook attacks
- finished making initMagicBitboards.
Completely finished.