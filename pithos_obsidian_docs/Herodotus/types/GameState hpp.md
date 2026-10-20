---
module: Herodotus
section: types
source: src/engines/herodotus/types/GameState.hpp
status: documented
---

# GameState.hpp

## Overview

This file holds the game state struct, which encapsulates important information pertaining to the game that cannot be derived solely from piece positions, such as whose turn it is, what special moves are available, and the counters for move history.

## Structure

```cpp
struct GameState {
    Color activeColor;           // The current active player.
    
    enum CastleBit : uint8_t {
        WHITE_KINGSIDE_CASTLE,
        WHITE_QUEENSIDE_CASTLE,
        BLACK_KINGSIDE_CASTLE,
        BLACK_QUEENSIDE_CASTLE
    };
    
    uint8_t castlingRights;      // 8 bits representing the currently allowable castles:
                                 // bit 0 = white kingside, bit 1 = white queenside,
                                 // bit 2 = black kingside, bit 3 = black queenside (0x1, 0x2, 0x4, 0x8).
                                 // Rights are lost when a king moves, or when the relevant rook moves or is captured.
                                 // At the start all four are available (0x0F). Used a bitmask to reduce memory overhead.
    
    int halfMoveClock;           // Counts the number of halfmoves (plies) since the last capture or pawn move,
                                 // resetting to 0 on those moves. Reaching 100 creates a drawn game (the 50-move rule).
                                 // ⚠️ Known gap: the engine currently only ever resets it and never increments it.
    
    int fullMoveNumber;          // Starts at 1 and increments after each of Black's moves.
    
    std::optional<Square> enPassant;  // The current en passant target square, if there is one available.
                                     // At any one time only one square may be en passanted, so a single variable is sufficient.
};
```

## Design notes

Because `GameState` contains everything about a position that piece positions do not, undo becomes trivial: save the whole struct before a move and restore it wholesale — which is exactly what [[BoardMemento hpp]] does.
