---
module: Herodotus
---

# Getting Started

## Overview

Herodotus is the chess engine of the Pithos project. Its code lives in `src/engines/herodotus/`, with tests in `src/engines/herodotus_testing/`.

## Prerequisites

- A C++20 compiler
- CMake >= 3.14
- Qt6 Widgets (required at configure time by the root `CMakeLists.txt`, even for engine-only builds)
- Internet access on first configure (Catch2 v3.5.2 is fetched via `FetchContent`)

## Build

From the repository root:

```sh
cmake -B build
cmake --build build --target herodotus
```

Useful targets:

| Target | Output |
| ------ | ------ |
| `herodotus` | The engine executable |
| `herodotus_test` | Catch2 test suite |
| `HerodotusCore` | Static library of the engine (no main) |
| `PithosCore` | Core board/game library the engine links against |

The repo may already contain a configured `build/` directory; rerun the build command to pick up changes.

## Run

```sh
./build/herodotus
```

This prints the starting board, the number of legal moves, and moves from the starting position. There is no UCI loop yet.

Run the tests with:

```sh
./build/herodotus_test
```

## Project layout

```
src/engines/herodotus/
├── core/        HerodotusEngine class
├── movegen/     Move generation (pawn/knight/bishop/rook/queen/king)
├── types/       Bitboard, Move, Square, GameState, BoardMemento
├── hash/        Zobrist hashing
└── utils/       Bitboard tables, magic bitboards
src/engines/herodotus_testing/   Catch2 tests (perft, movegen, special moves, etc.)
```
