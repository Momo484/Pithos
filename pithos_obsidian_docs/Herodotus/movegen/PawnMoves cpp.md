---
module: Herodotus
section: movegen
source: src/engines/herodotus/movegen/PawnMoves.cpp
status: documented
---

# PawnMoves.cpp

## Overview

This file contains the function implementation for generating all pawn moves: single pushes, double pushes, captures, promotions, and en passant. The logic is written out twice, once for white and once for black, since pawns are the only piece where the direction of travel depends on colour.

## Structure

### `generatePawnMoves`

```cpp
void generatePawnMoves(const PositionBitboards &pieces, const Mailbox &mailbox,
                       Bitboard friendly, Bitboard enemy, Bitboard all,
                       Color side, const GameState &state,
                       std::vector<Move> &out);
```

Given the bitboards, mailbox, game state and an output vector, this function generates every pseudo-legal pawn move for the given side. It does so by branching on colour, then running the same set of steps for each branch:

- **Single pushes** — shift the pawn bitboard one square forward (`<< 8` for white, `>> 8` for black) and mask with `empty`. The promotions are split off first by masking with the promotion rank and pushing all four promotion pieces per destination, then the remainder is masked with `~RANK_8` (or `~RANK_1`) so the non-promotion pushes stay separate.
- **Double pushes** — take the single pushes that landed on rank 3 (rank 6 for black), shift them forward again, and mask with `empty` so blocked doubles are discarded. These are pushed with `isPawnDoublePush` set, which is how the engine knows to record the en passant target.
- **Captures** — shift the pawn bitboard diagonally (`<< 9` east and `<< 7` west for white, `>> 7` and `>> 9` for black), mask off the wrapping file with `~FILE_A`/`~FILE_H`, and keep only squares occupied by enemy pieces.
- **En passant** — if the game state has an en passant square, shift *from* the en passant square backwards (`>> 9` and `>> 7` for white) to find pawns that could take it, again with file masks to prevent wrap around, then push each one with `isEnPassant` set.

The `pushCapture` lambda handles captures so the same code path serves both the diagonal captures and (conceptually) any destination we need to inspect. It reads `mailbox[toSq]` to find what is being captured, and if the destination sits on the promotion rank it pushes all four promotion moves (knight, bishop, rook, queen), otherwise a single capture move.

## Design notes

- The mailbox is what makes captures cheap here — `mailbox[toSq]` gives the captured piece type in O(1) instead of having to scan the enemy bitboards to work out which piece got hit. Note the captured piece is only recorded for actual captures; for en passant I leave it as `Piece::PAWN` and let make move work out where the captured pawn actually sits.
- Pawns are the one piece with no attack table. Their movement is simple enough that raw shifts with file masks do the job, so there is nothing precomputed to look up.
- The white and black branches are fully mirrored copies of each other. I wrote it this way to keep each colour readable in isolation, but it is the obvious candidate for a refactor if I ever want to unify them with a direction multiplier.
- Promotions are emitted as four distinct moves per destination rather than one move with a promotion field set later, so the rest of the pipeline can treat each promotion candidate as its own move.
