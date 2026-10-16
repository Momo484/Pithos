## Overview 
This is a simple struct that holds
- Move move -> the previous most played on the board.
- GameState state -> the previous game state on the board.
BoardMemento is used to hold the move history of a chess game, this is critical for correctly restoring a previous game state.