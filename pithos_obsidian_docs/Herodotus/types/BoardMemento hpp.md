---
module: Herodotus
section: types
source: src/engines/herodotus/types/BoardMemento.hpp
---

# BoardMemento.hpp

## Overview
This is a simple struct that holds:
- `Move move` — the most recently played move.
- `GameState state` — the game state as it was *before* that move was applied.

## Structure
Just the two fields above; no methods or extra logic.

## Design notes
`BoardMemento` is used to hold the move history of a chess game (the engine keeps a stack of them), which is critical for correctly restoring a previous game state — see `makeMove`/`undoMove` in the core engine. Note the struct is already comment-free in the source; see [[Move hpp]] and [[GameState hpp]] for what the two fields contain.
