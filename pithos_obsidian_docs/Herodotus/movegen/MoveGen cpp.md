---
module: Herodotus
section: movegen
source: src/engines/herodotus/movegen/MoveGen.cpp
status: documented
---

# MoveGen.cpp

## Overview

This file contains the function implementations for generating pseudo-legal moves, checking if the king is checked, validating moves, and converting an array of pseudo-legal moves to legal moves. Essentially, this is the implementation details of the whole move generation pipeline.

## Structure

### `generatePseudoLegalMoves`

```cpp
void generatePseudoLegalMoves(Engine &engine, std::vector<Move> &out)
```

Given the engine and an output vector, this function will generate all pseudo-legal moves from the engine's current position. It does so by calling the respective `generate` functions for each piece type and passing in the required parameters.

### `isKingChecked`

```cpp
bool isKingChecked(Engine &engine, Color color)
```

Given the engine and a color to check, this function determines if the king is currently checked. It does so by grabbing bishop and rook attacks from its positive masked when non-friendly pieces (magic bitboards) and knight attacks. From there, it checks that each attack/threat is of the correct type using the mailbox. Finally, it checks if there are any pawn threats with some hardcoded bitmasks. This returns a bool.

### `pseudoToLegalMoves`

```cpp
std::vector<Move> pseudoToLegalMoves(Engine &engine, std::vector<Move> &pseudoMoves)
```

This function returns an array of legal moves. It does so by first accepting an array of pseudo-legal moves and checking each one with the `validateMove` function that follows.

### `validateMove`

```cpp
bool validateMove(Engine &engine, const Move &move)
```

Given the engine and a move, this function checks if the move is valid. It does so currently by treating castling moves and others separately. Essentially, this function simulates the move, checks if the king was checked, then undoes the move, returning the result. At its current state, a move is legal when it doesn't result in the friendly king being checked.

## Design notes

- It is known that the current `validateMove` implementation is too slow, hinging on the fact that `makeMove` and `undoMove` do a lot of unnecessary bookkeeping. It is planned to be refactored with a light version of make and undo move.
