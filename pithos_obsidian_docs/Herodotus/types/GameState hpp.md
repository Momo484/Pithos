---
module: Herodotus
section: types
source: src/engines/herodotus/types/GameState.hpp
---

# GameState.hpp

## Overview
This file holds the game state struct, which encapsulates important information pertaining to the game that cannot be derived solely from piece positions, such as whose turn it is, what special moves are available, and the counters for move history.

## Structure
The `GameState` struct holds the following fields:
- `Color activeColor` — the current active player.
- `enum CastleBit : uint8_t` — named constants for the bits of the `castlingRights` field below. (Re my note about it "possibly could be static": a nested enum is a type, not a per-instance member, so it already costs nothing per `GameState` — nesting it only scopes the names to `GameState::`.)
- `uint8_t castlingRights` — 8 bits representing the currently allowable castles: bit 0 = white kingside, bit 1 = white queenside, bit 2 = black kingside, bit 3 = black queenside (`0x1`, `0x2`, `0x4`, `0x8`). Rights are lost when a king moves, or when the relevant rook moves or is captured. At the start all four are available (`0x0F`). I used a bitmask to reduce memory overhead.
- `int halfMoveClock` — counts the number of halfmoves (plies) since the last capture or pawn move, resetting to 0 on those moves. Reaching 100 creates a drawn game (the 50-move rule). ⚠️ Resolved open question: the intent is for this to increment on every move, but checking `makeMove`, the engine currently only ever resets it and never increments it — a known gap to fix in the core engine.
- `int fullMoveNumber` — starts at 1 and increments after each of Black's moves.
- `std::optional<Square> enPassant` — the current en passant target square, if there is one available. At any one time only one square may be en passanted, so a single variable is sufficient.

## Design notes
Because `GameState` contains everything about a position that piece positions do not, undo becomes trivial: save the whole struct before a move and restore it wholesale — which is exactly what [[BoardMemento hpp]] does.
