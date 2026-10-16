## Overview 
This file holds the square representation and corresponding utilities for chess board positions. A square represents a single location on the 8x8 chess board represented by rank (rows) and file (columns).
***
### Data Structure 
The Struct Square data structure holds two variable fields both uint8_t, one for rank and one for file. Furthermore, the struct holds methods;
- To determine if the square is valid.
- To determine if one is equivalent to another one.
- To determine if one is not equivalent to another one.
- To convert the square to its bit board representation. 
- To convert from index in bit board to square.
There is also a hashing function for the square defined. Furthermore there is a static const associated with the Square struct for the number of square on the board. 
### Choice of uint8_t
I chose uint8_t for the sake of saving memory. I don't know if it actually did anything, I don't know how memory is padded in c++.
### Choice of struct over class for Square 
I chose struct initially as I had not plans for built in methods (I didn't know you could do them in a struct), but when i realised i needed them I didn't see reason to change. The privacy of a variable is not of concern by design and inheritance does not seem likely. In essence a square is a passive data structure that doesn't inherently have behaviour, it has further data that can be extracted from it, but in of it self, it does not do anything. 
