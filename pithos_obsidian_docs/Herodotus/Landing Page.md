---
module: Herodotus
---

# Herodotus Engine

Hello! This is the landing page for the Herodotus Engine. I'm writing these obsidian docs to help myself keep track of the project, the design decisions I've made, and to quickly reference/understand code after stretches of not touching the project.

[[Getting Started]]

## Notes
The notes folder contains markdown files from previous uses of AI to refactor the codebase. I used AI during this project for tasks such as refactoring or making codebase-wide changes that I couldn't be bothered to do myself, such as changing all uses of raw arrays to std::array for safety and use with constexpr. I have done my best to understand all of the code that is written here, however, and written as much as I could myself (probably why the engine is so slow :)))

## Types
To start building our understanding of the project, it would probably be best to begin with the different types/structs that have been defined in this code base, roughly in dependency order:
[[Bitboard hpp]]
[[Square hpp]]
[[Types hpp]]
[[Move hpp]]
[[GameState hpp]]
[[BoardMemento hpp]]

## Utilities
Precomputed lookup tables and magic bitboard generation used by move generation.
[[BitboardTables hpp]]
[[BitboardTables cpp]]
[[MagicBitboards hpp]]
[[MagicBitboards cpp]]

## Move generation
Pseudo-legal and legal move generation, split per piece type.
[[MoveGen hpp]]
[[MoveGen cpp]]
[[PawnMoves hpp]]
[[PawnMoves cpp]]
[[KnightMoves hpp]]
[[KnightMoves cpp]]
[[BishopMoves hpp]]
[[BishopMoves cpp]]
[[RookMoves hpp]]
[[RookMoves cpp]]
[[QueenMoves hpp]]
[[QueenMoves cpp]]
[[KingMoves hpp]]
[[KingMoves cpp]]

## Core
The engine class that owns the board, game state and move history.
[[HerodotusEngine hpp]]
[[HerodotusEngine cpp]]

## Hashing
Zobrist hashing for compact position identification.
[[ZobristHash hpp]]
[[ZobristHash cpp]]

## Entry point
[[main cpp]]
