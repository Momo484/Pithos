## Overview
This file holds the game state struct, which encapsulates important information pertaining to the game that cannot be derived solely from piece positions, such as whoes turn it is, what special moves are available, and counters for move history.
***
### Data Structure 
The Game State Struct holds the following variables.
- Color activeColor -> current active player
- enum Castlebit : uint8_t -> this is enum (could be static possibly?) that helps remember the castling constants for the following castlingRights variable.
- uint8_t castlingRights ->  These are 8 bits used represent the current allowable castles, this changes when either king or rooks move, 0x1,0x2,0x4,0x8 are the bits used. I used this to reduce memory overhead (i don't know if it actually does anything).
- int halfMoveClock -> counts the number of halfmoves since the last capture or pawn moves, a half move increments whenever a move is made? This creates a drawn game if it reaches 100.
- int fullMoveNumber -> starts at 1 and increments after each of black moves.
- std::optional\<Square> enPassant -> holds the current en passant target square if there is one available, at any one time there is only one square that may be enpassented, so a single variable is sufficient.