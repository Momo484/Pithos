## Overview
This file contains the core definitions and enumerations for the Herodotus chess engine.
***
### Enum Piece
This is an enum representing the 6 possible piece types with an additional field that keeps the count of the total number of piece types. The order is as follows: pawn, knight, bishop, rook, queen, king, num_pieces (0-indexed). I created this enum so that the code indexing into bit board arrays and such would leave no room for confusion. 
### Enum Color
American spelling I know, I think it is mean't to be standard in the space to use american spelling so i stuck with it. This again exists for the same reason as the piece enum and after often used hand in hand, the order is: white, black, num_colors (0-indexed).
### Using PieceBitboards
This file also takes the chance to define what a piecebitboard is represented by, in this case being and array of bitboards, of size 6 (or num_pieces). I chose an array over a vector as the sizing of this is static, and I beleive that lends itselft to a lower memory overhead and performance gains. 
### Using PositionBitboards
Similarly, this defines PositionBitboards as the array of all bitboards for each piece, for each colour. Thus it is an array of PieceBitboards of size num_colors. Again the reasoning for array vs vector is the same.
### Mailbox
Here the mailbox data structure is also defined. In brief, a mailbox is an array that maps a square (by index \[0,64]) to a piece (or num_pieces if empty), this serves as a useful way to identify the piece at a position, when normal bitboards do not.
