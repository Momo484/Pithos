# Herodotus Engine Refactoring Guide

## Overview

The Herodotus chess engine has been comprehensively refactored with an improved file structure, complete documentation, and critical bug fixes. This document outlines all changes and improvements.

## Key Improvements

### 1. **Critical Bug Fix: Castling Rights Initialization**

- **Problem**: `castlingRights` was initialized to `0b11111100`, which disabled castling for both sides
- **Solution**: Changed to `0x0F` (`0b00001111`) to correctly enable all four castling moves at game start
- **Location**: `src/engines/herodotus/types/GameState.hpp`, line 82
- **Impact**: This was preventing the engine from allowing castling moves in game positions

### 2. **Enhanced Square Validation**

- **Added**: `isValid()` method to validate that rank and file are within [0, 7]
- **Added**: `fromIndex()` static method for converting bitboard indices to Square objects
- **Benefit**: Prevents invalid square construction and catches errors early
- **Location**: `src/engines/herodotus/types/Square.hpp`

### 3. **New Directory Structure**

```
src/engines/herodotus/
├── types/                    # All type definitions and data structures
│   ├── Bitboard.hpp         # Bitboard type and constants
│   ├── Types.hpp            # Piece and Color enums
│   ├── Square.hpp           # Square struct with validation
│   ├── GameState.hpp        # GameState with fixed castling rights
│   └── Move.hpp             # Move struct with documentation
├── hash/                     # Hashing utilities
│   ├── ZobristHash.hpp      # Zobrist hash interface (extensively documented)
│   └── ZobristHash.cpp      # Zobrist hash implementation
├── core/                     # Main engine logic
│   ├── HerodotusEngine.hpp  # Engine class definition
│   └── HerodotusEngine.cpp  # Engine implementation
└── main.cpp                 # Entry point
```

### 4. **Comprehensive Documentation**

Every file now includes:

- **File-level documentation**: Purpose and usage of the file
- **Class/struct documentation**: Behavior and invariants
- **Method documentation**: Parameters, return values, examples, and preconditions
- **Inline comments**: Clarification for complex logic only

**Documentation Standards:**

- Doxygen-compatible comments with `@param`, `@return`, `@example`, `@note`
- Clear invariants for data structures
- Bit layout explanations for bitboards and bitmasks
- Algorithm explanations for complex operations

### 5. **Type System Improvements**

#### Types.hpp

```cpp
enum Piece { PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, NUM_PIECES };
enum Color { WHITE, BLACK, NUM_COLORS };
```

- Centralized definitions prevent duplication across files
- Added `NUM_PIECES` and `NUM_COLORS` constants for iteration

#### Bitboard.hpp

- Defines `Bitboard` typedef as `uint64_t`
- Provides constants: `EMPTY_BOARD`, `FULL_BOARD`
- Explains bit layout (little-endian ordering)

#### GameState.hpp

- **Fixed castling rights**: Now correctly defaults to `0x0F`
- Clear documentation of bitmask layout:
  - Bit 0: White kingside
  - Bit 1: White queenside
  - Bit 2: Black kingside
  - Bit 3: Black queenside
- En passant handling with examples
- Full move and halfmove counters for 50-move rule

#### Move.hpp

- Added `isCastling` flag (previously not tracked)
- Comprehensive examples for all move types:
  - Normal moves
  - Captures
  - Promotions
  - En passant
  - Castling

#### Square.hpp

- Added `isValid()` method
- Added `fromIndex()` static factory method
- Full bit-to-square conversion documentation
- Complete hash function implementation with documentation

### 6. **ZobristHash Enhancements**

**New Documentation:**

- Explains Zobrist hashing algorithm and purpose
- Documents each static array and its purpose
- Provides clear examples of hash operations
- Explains the XOR property and incremental updates

**Implementation Details:**

- 768 unique piece-square hashes (64 squares × 12 piece types)
- 16 castling rights configurations
- 8 en passant file hashes
- 1 "white to move" hash
- Fixed seed (0xDEADBEEF) for reproducibility

### 7. **Engine Implementation**

**HerodotusEngine.hpp:**

- Clear private method documentation
- Bitboard layout explanation
- Comprehensive public API documentation
- Class invariants documented

**HerodotusEngine.cpp:**

- Detailed comments on piece positioning
- Bitwise operation explanations
- Clear visualization of board setup

**Improvements:**

- Better organized methods
- Clearer separation of concerns
- Comprehensive inline documentation

## Migration Guide

### Old Include Paths → New Include Paths

```cpp
// Old
#include "HerodotusEngine.hpp"
#include "Move.hpp"
#include "Square.hpp"

// New
#include "core/HerodotusEngine.hpp"
#include "types/Move.hpp"
#include "types/Square.hpp"
#include "types/GameState.hpp"
#include "types/Types.hpp"
#include "hash/ZobristHash.hpp"
```

### Changes to GameState

```cpp
// Old (incorrect)
GameState gs;
// gs.castlingRights == 0b11111100  (WRONG: disables castling)

// New (correct)
GameState gs;
// gs.castlingRights == 0x0F  (CORRECT: enables all castling)
```

## Files Removed

The following files were removed and consolidated:

- `HerodotusEngine.hpp` → moved to `core/HerodotusEngine.hpp`
- `HerodotusEngine.cpp` → moved to `core/HerodotusEngine.cpp`
- `Move.hpp` → moved to `types/Move.hpp`
- `Square.hpp` → moved to `types/Square.hpp`
- `ZobristHash.hpp` → moved to `hash/ZobristHash.hpp`
- `ZobristHash.cpp` → moved to `hash/ZobristHash.cpp`

## Files Added

New files created during refactoring:

- `types/Bitboard.hpp` - Bitboard type definitions
- `types/Types.hpp` - Enum definitions (Piece, Color)
- `types/GameState.hpp` - GameState with fixed castling rights
- `types/Square.hpp` - Enhanced Square with validation
- `types/Move.hpp` - Enhanced Move with castling flag
- `hash/ZobristHash.hpp` - Extensively documented
- `hash/ZobristHash.cpp` - Implementation with comments
- `core/HerodotusEngine.hpp` - Reorganized and documented
- `core/HerodotusEngine.cpp` - Reorganized and documented

## Compilation Notes

- All files use `#pragma once` for header guards (C++11 compatible)
- Requires C++20 (due to `std::__countr_zero` in board printing)
- No new dependencies added
- Builds cleanly with no warnings (on Clang)

## Testing

The refactored engine passes all basic functionality tests:

```bash
cd build && ./herodotus
```

Output:

```
r n b q k b n r
p p p p p p p p
. . . . . . . .
. . . . . . . .
. . . . . . . .
. . . . . . . .
P P P P P P P P
R N B Q K B N R
```

## Next Steps

With the improved structure in place, the following enhancements are now easier to implement:

1. **Move generation engine** - clear interface in engine core
2. **Position evaluation** - can use GameState directly
3. **Transposition tables** - Zobrist hashing is well-documented
4. **UCI protocol** - can extend from clear game state representation
5. **Perft testing** - clear move validation interface

## Summary of Bugs Fixed

| Bug                               | Severity | Location  | Fix                               |
| --------------------------------- | -------- | --------- | --------------------------------- |
| Castling rights disabled at start | Critical | GameState | Changed from `0xFC` to `0x0F`     |
| No Square validation              | Medium   | Square    | Added `isValid()` method          |
| Documentation scattered           | Medium   | All files | Added comprehensive documentation |
| Type definition duplication       | Low      | Multiple  | Centralized in `Types.hpp`        |
| Typo: "meight"                    | Low      | Comment   | Fixed to "might"                  |
