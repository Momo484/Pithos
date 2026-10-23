---
module: Herodotus
section: core
source: src/engines/herodotus/core/HerodotusEngine.cpp
status: stub
---
# HerodotusEngine cpp
## Overview
This file contains the implementation of Herodotus engine's core functionality. 
## Structure
```cpp
void HerodotusEngine::setupStartingPosition()
```
-> This is a function that initialises all the 0ULL bitboards to the standard starting state. The values are just hardcoded in, nothing special.
```cpp
void HerodotusEngine::clearBoard()
```
-> This function goes through every bitboard and clears it to 0ULL.
```cpp
Bitboard HerodotusEngine::getWhitePieces()
Bitboard HerodotusEngine::getBlackPieces()
Bitboard HerodotusEngine::getAllPieces()
```
-> These functions do the necessary bitwise OR operations to retrieve the corresponding bitboard for these piece sets. 
```cpp
void HerodotusEngine::initialise()
```
-> This is a function intended to be called when starting the engine.
1. It first clears the board.
2. Then sets up the default starting position.
3. Initialises game state.
4. Syncs the mailbox.
5. Initialises the Zobrist hash
6. Initialises the magic bitboards.
```cpp
void HerodotusEngine::loadFEN(const std::string &fen)
```
-> This function loads in a board position from a given FEN.
1. It first clears the board.
2. It then sets the boards state.
3. Then parses the FEN by reading each character and determining where and how the pieces are placed.
4. It continues by reading the active colour, castling rights, enpassant target square, and move counters.
5. Finally, it syncs the mailbox, initialises the Zobrist Hash and initialises the magic bitboards.
```cpp
void HerodotusEngine::initialiseHash()
```
-> This function initialises the Zobrist hash to the correct state for the start of the game. See [[ZobristHash cpp]] to see how the Zobrist hashing is set up. It first calls ZobristHash:initialise, then for every piece on the board, it sets the zobrist hash accordingly.
```cpp
void HerodotusEngine::syncMailbox()
```
-> This function sets up the mailbox so that it is correct. It loops through every piece bitboard and sets the mailbox entry for occupied squares with that piece.
```cpp
void HerodotusEngine::printBoardState()
```
-> This function prints the board state, pretty simple.
```cpp
void HerodotusEngine::makeMove(Move move)
```
-> This is one of the most important functions in the engine, It does all the neccesary actions to "make" a move within the system, this includes updating the mailbox, pushing to history, updating bitboards, updating the hash, and other game state book keeping.
1. First place the current game state on stack, and extract relavent squares, and pieces from the move.
2. Then update the hash as much as possible
3. Update the clocks.
4. Then, depending on the move type do the neccesary actions...
5. re-update the hash with the new values.
```cpp
void HerodotusEngine::undoMove(void)
```
-> essentially does all of the actions of make move in reverse to "undo" the last played move.
```cpp
std::vector<Move> HerodotusEngine::generateAllLegalMoves()
```
-> This function generates all legal moves given the current board state, it does so by calling the private friend methods, generatePseudoLegalmoves and pseudoToLegalMoves.
## Design notes
- Make move and Undo Move are currently too heavy for move generation. It is planned to be replaced (in move generation) with lean functions that operate under the assumption that they are played one after the other, resulting in no change in state.  In this case, we do not have to update the mailbox, hashing, or state, saving time.