---
module: Herodotus
section: movegen
source: src/engines/herodotus/movegen/MoveGen.hpp
status: stub
---
# MoveGen hpp

## Overview
This file simply prototypes the most important move generation functions. It in a friend namespace i believe of the Herodotus Engine class from [[HerodotusEngine hpp]].
## Structure
This file contains 3 function prototypes 
- generatePseudoLegalMoves
- isKingChecked
- pseudoToLegalMoves
- validateMove
All functions are explained in [[MoveGen cpp]]. 
## Design notes
This file contains friend function prototypes of Herodotus Engine. I segmented the code in this way to prevent the Herodotus Engine cpp file from blowing up in size, segmented away move generation logic. It needed the friend status to access private members of the engine, such as the mailbox, and such after bing provided the engine by reference. 
