## Overview
This file contains the move representation for the chess engine. A move is a struct that encapsulates all the information about a single chess move, so that from a given board state the exact move can be replicated, and importantly, from the state after the move, we can restore most of the previous state of the board.
***
### Data Structure 
The move struct holds the following variables
- Color color -> holds the colour of the moving piece. [[Types hpp]]
- Piece piece -> holds the type of piece being moves. [[Types hpp]]
- Square from -> holds the starting square of the move. [[Square hpp]]
- Square to -> holds the destination square of the move. [[Square hpp]]
- std::optional\<Piece> captured -> an optional field that holds the piece captured, if a piece was captured on this move. https://cppreference.com/cpp/utility/optional  
- std::optional\<Piece> promotion -> similarly holds the piece type the move wishes to promote to, if they could.
- bool isEnPassant -> simply that, determines if the move was an enpassant (important for reversing a move).
- bool isCastling -> determines if the move was a castling move, important ideas, 'piece' is a always king, 'from' is always king position, and type of move (queenside or kingside) is recovered from destination square. 
- bool isPawnDoublePush -> simply that, determines this was from a double pawn push, on further thought this is slightly redundant, and could be determined from comparing from and to squares.
