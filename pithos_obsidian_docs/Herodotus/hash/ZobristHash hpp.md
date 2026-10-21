---
module: Herodotus
section: hash
source: src/engines/herodotus/hash/ZobristHash.hpp
status: stub
---
# ZobristHash hpp
## Overview
Zobrist hashing is a technique developed by Albert Zobrist and it is used in this chess engine for uniquely recording the board state at any given point in the chess match, this is important so that the engine can know when the 3 move repetition or such occurs.  This hpp file contains the static variable and function declarations implemented in [[ZobristHash cpp]]. 
## Structure
### Private
```cpp
static bool initialsed;
```
-> This indicates weather the hash has been initialised during the engine startup. (probably could be removed).
```cpp
static constexpr int NUM_PIECES = 12
static constexpr int NUM_SQUARES = 64
```
-> This is self explanatory. 
```cpp
static std::array<std::array<uint64_t, NUM_PIECES>, NUM_SQUARES> pieceSquareTable;
```
-> This stores a unique random 64 bit hash for each combination of piece and square, effectively allowing for the encoding of every single board state in one 64 bit unsigned integer.
```cpp
static uint64_t whiteToMove
```
-> Similar to the above, indicates which side's turn it is to move. 
```cpp
static std::array<uint64_t, 16> castlingRights;
static std::array<uint64_t, 8> enPassantFiles;
```
-> Same philosophy to the two variables above.
```cpp
static int getPieceIndex(char symbol);
```
-> Maps a piece character to its index, white is uppercase and occupies 0-5, black occupies 6-11
### Public 
```cpp
static void initialise()
```
-> This initialises all tables with random values, it does not have any additional affects after it's first call. 
```cpp
static uint64_t getPieceHash(Piece piece, Color pieceColor, Square sq);
```
-> Given a piece, Color and square, this returns the hash value from the pieceSquareTable for XORing. 
```cpp
static uint64_t getCastlingHash(bool whiteKingSide, bool whiteQueenSide, bool blackKingSide, bool blackQueenSide);
static uint64_t getEnPassantHash(Square epTarget);
static uint64_t getToMoveHash(Color color);
```
-> Same idea as above.
## Design notes
- This implementation was largely made by AI, upon closer inspection I believe it is incorrect as it makes no attempt to ensure that each random number is unique from all others. 
	- I will be refactoring this soon, using a simple set to check for duplicates.