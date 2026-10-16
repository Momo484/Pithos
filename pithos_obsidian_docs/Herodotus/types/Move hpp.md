---
module: Herodotus
section: types
source: src/engines/herodotus/types/Move.hpp
---

# Move.hpp

## Overview
This file contains the move representation. A `Move` struct encapsulates all the information about a single chess move, so that from a given board state the exact move can be replicated, and importantly, from the state after the move we can restore most of the previous state of the board.

## Structure
The `Move` struct holds the following fields:
- `Color color` — the colour of the moving piece. [[Types hpp]]
- `Piece piece` — the type of piece being moved. [[Types hpp]]
- `Square from` — the starting square of the move. [[Square hpp]]
- `Square to` — the destination square of the move. [[Square hpp]]
- `std::optional<Piece> captured` — holds the piece captured, if a piece was captured on this move. See [cppreference: std::optional](https://cppreference.com/cpp/utility/optional). For en passant, note the captured pawn is *not* on `to`.
- `std::optional<Piece> promotion` — similarly holds the piece type the move wishes to promote to, if it could.
- `bool isEnPassant` — simply that: whether the move was an en passant (important for reversing a move).
- `bool isCastling` — whether the move was a castling move. Key ideas: `piece` is always king, `from` is always the king's position, and the type of move (queenside or kingside) is recovered from the destination square.
- `bool isPawnDoublePush` — whether this move came from a double pawn push. On further thought this is slightly redundant and could be determined by comparing the `from` and `to` squares.

## Design notes
Colour and piece type are stored explicitly rather than looked up on the board, which makes each move self-contained — handy for undo, move history, and validation without board access. Every special-move flag exists mainly so `undoMove` can reverse the move exactly; see [[BoardMemento hpp]] for how moves are stored alongside the state they replaced.
