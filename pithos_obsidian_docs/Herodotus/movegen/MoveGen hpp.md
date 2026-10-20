---
module: Herodotus
section: movegen
source: src/engines/herodotus/movegen/MoveGen.hpp
status: documented
---

# MoveGen.hpp

## Overview

This file simply prototypes the most important move generation functions. It is in a friend namespace (I believe) of the Herodotus Engine class from [[HerodotusEngine hpp]].

## Structure

This file contains 3 function prototypes:

- `generatePseudoLegalMoves` — Generates all pseudo-legal moves for the current position
- `isKingChecked` — Determines if a king of the specified color is under attack
- `pseudoToLegalMoves` — Filters pseudo-legal moves to only legal moves
- `validateMove` — Checks if a specific move is valid

All functions are explained in [[MoveGen cpp]].

## Design notes

This file contains friend function prototypes of Herodotus Engine. I segmented the code in this way to prevent the Herodotus Engine cpp file from blowing up in size, segmenting away move generation logic. It needed the friend status to access private members of the engine, such as the mailbox, and such after being provided the engine by reference.
