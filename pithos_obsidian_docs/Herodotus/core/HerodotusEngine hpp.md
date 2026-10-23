---
module: Herodotus
section: core
source: src/engines/herodotus/core/HerodotusEngine.hpp
status: stub
---
# HerodotusEngine hpp
## Overview
This is the header file for the Herodotus engine class, which is the main class for the engine, it contains the information pertaining the board state and the functions required to make, undo, and generate moves. 
## Structure
### Misc declaration
```cpp
namespace MoveGen {
void generatePseudoLegalMoves(HerodotusEngine &engine, std::vector<Move> &out);
bool isKingChecked(HerodotusEngine &engine, Color side);
std::vector<Move> pseudoToLegalMoves(HerodotusEngine &engine,
                                     const std::vector<Move> pseudoLegalMoves);

bool validateMove(HerodotusEngine &engine, Move move);

} // namespace MoveGen
```
-> This defines a namespace prior to use, this specifically is used for move generation. The functions defined here are friend functions that have access to the private state contained in the engine.
### Data Members
```cpp
PositionBitboards pieces;
```
-> This is a critical part to the engine, this holds all the bitboards for all the pieces, see [[Types hpp]] for a description.
```cpp
GameState gameState;
```
-> This holds the current game state. Whose turn, castling rights, move counters, etc. See [[GameState hpp]] for more info.
```cpp
std::stack<BoardMemento> history;
```
-> This is the history stack, it holds board mementos in order of previous board states, see [[BoardMemento hpp]] for info in whats containe.
```cpp
Mailbox mailbox;
```
->  The mailbox, holding what piece is in which position. See [[Types hpp]] for more info.
```cpp
  friend void MoveGen::generatePseudoLegalMoves(HerodotusEngine &engine,
                                                std::vector<Move> &);
  friend bool MoveGen::isKingChecked(HerodotusEngine &engine, Color side);
  friend std::vector<Move>
  pseudoToLegalMoves(HerodotusEngine &engine,
                     const std::vector<Move> pseudoLegalMoves);

  friend bool MoveGen::validateMove(HerodotusEngine &engine, Move move);
```
-> here the friend functions are declared, see [[MoveGen hpp]] for info on these functions, and [[MoveGen cpp]] for implementation details.
```cpp
uint64_t zobristHash = 0;
```
-> This is the zobrist hash for the engine. See [[ZobristHash hpp]] for more info.
```cpp
std::unordered_map<uint64_t, int> boardStateHashCount;
```
-> This data member holds a count for every time a particular zobrist hash has been seen, essentially holding count for 3 move repetition.
### Private methods
```cpp
  Bitboard getWhitePieces();
  Bitboard getBlackPieces();
  Bitboard getAllPieces();
```
-> This are self explanatory getters.
```cpp 
void syncMailbox();
void initialiseHash();
void setupStartingPosition();
void clearBoard();
```
-> These are function prototypes, explained in [[HerodotusEngine cpp]].
### Public methods
```cpp 
  void initialise();
  void printBoardState();
  void makeMove(Move move);
  void undoMove(void);
  std::vector<Move> generateAllLegalMoves();
  void loadFEN(const std::string &fen);
```
-> These are function prototypes available to the public, see [[HerodotusEngine cpp]] for implementation details.
### Public Accessors
```cpp
  const GameState &getGameState() const { return gameState; }
  uint64_t getZobristHash() const { return zobristHash; }
  const Mailbox &getMailbox() const { return mailbox; }
  Bitboard getPieceBitboard(Color color, Piece piece) const {
    return pieces[color][piece];
  }
  size_t getHistorySize() const { return history.size(); }
```
-> accessors, pretty self explanatory.
## Design notes
- For Zobrist hashing, the full make move and undo move contribute, while fast versions will not. Also 3 move repetion is not checked yet.
