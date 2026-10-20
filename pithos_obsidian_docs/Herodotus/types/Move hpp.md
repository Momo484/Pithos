---
module: Herodotus
section: types
source: src/engines/herodotus/types/Move.hpp
status: documented
---

# Move.hpp

## Overview

This file contains the move representation. A `Move` struct encapsulates all the information about a single chess move, so that from a given board state the exact move can be replicated, and importantly, from the state after the move we can restore most of the previous state of the board.

## Structure

```cpp
struct Move {
    Color color;              // The colour of the moving piece. [[Types hpp]]
    Piece piece;              // The type of piece being moved. [[Types hpp]]
    Square from;              // The starting square of the move. [[Square hpp]]
    Square to;                // The destination square of the move. [[Square hpp]]
    std::optional<Piece> captured;  // Holds the piece captured, if any. For en passant, note the captured pawn is *not* on `to`.
    std::optional<Piece> promotion; // Holds the piece type the move promotes to, if applicable.
    bool isEnPassant;         // Whether the move was an en passant (important for reversing a move).
    bool isCastling;          // Whether the move was castling. Key ideas: `piece` is always king, `from` is always the king's position, and the type of move (queenside or kingside) is recovered from the destination square.
    bool isPawnDoublePush;    // Whether this move came from a double pawn push.
};
```

## Design notes

Colour and piece type are stored explicitly rather than looked up on the board, which makes each move self-contained — handy for undo, move history, and validation without board access. Every special-move flag exists mainly so `undoMove` can reverse the move exactly; see [[BoardMemento hpp]] for how moves are stored alongside the state they replaced.
