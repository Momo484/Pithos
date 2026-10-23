---
module: Herodotus
---

# Getting Started

## Overview

Herodotus is the chess engine of the Pithos project. Its code lives in `src/engines/herodotus/`, with tests in `src/engines/herodotus_testing/`.

## Prerequisites

- A C++20 compiler (GCC, Clang, or AppleClang)
- CMake >= 3.14
- Qt6 Widgets (required at configure time by the root `CMakeLists.txt`, even for engine-only builds)
- Internet access on first configure (Catch2 v3.5.2 is fetched via `FetchContent`)

## Build

By default, **Release mode with aggressive optimizations** (`-O3 -march=native -DNDEBUG` + thin LTO). This yields ~3.5x NPS improvement over unoptimized builds (from ~1.5M to ~5.4M nodes/sec on depth 6 perft).

### Performance tuning

The default optimization flags are tuned for your local machine (`-march=native`). For maximum performance, these can be further tuned by:

1. **Enabling full LTO** (slower link times but better optimizations):
   ```sh
   # Apple Silicon/MacOS - replace -flto=thin with -flto
   add_link_options(-flto)
   
   # Linux/GCC
   add_link_options(-flto)
   ```

2. **Adding vectorization hints** (if your compiler supports it):
   ```sh
   add_compile_options(-mvectorize-all-loop-carries)  # GCC/Clang specific
   ```

3. **Using PGO (Profile-Guided Optimization)** for search workloads:
   ```sh
   # First, instrument the build
   cmake -B build_pgo -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-fprofile-instr-generate"
   cmake --build build_pgo --target herodotus_test
   ./build_pgo/herodotus_test
   
   # Then, use the profile data to optimize
   cmake -B build_pgo_use -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-fprofile-use"
   cmake --build build_pgo_use
   ```

### Debug builds

For debugging or profiling:

```sh
cmake -B build_debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build_debug --target herodotus
```

Or override the default in a single configure run:

```sh
cmake -B build -DCMAKE_BUILD_TYPE=Debug
```

### Build targets

| Target | Output | Description |
| ------ | ------ | ----------- |
| `herodotus` | Engine executable | Standalone CLI that prints board and legal moves |
| `herodotus_test` | Catch2 test suite | Perft, movegen, special moves, king check tests |
| `HerodotusCore` | Static library | Engine core (no main) for linking into other projects |
| `PithosCore` | Static library | Core board/game logic that Herodotus links against |

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
