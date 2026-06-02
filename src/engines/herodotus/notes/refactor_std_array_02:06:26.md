# std::array Refactor — 02:06:26

## Rationale

Replaced all raw C-style arrays with `std::array` for:

- **Type safety** — no implicit decay to pointer, bounds-aware interfaces
- **constexpr compatibility** — enables compile-time table generation (e.g., `BitboardTables::Between`)
- **Consistent C++ style** — zero-overhead abstraction, same memory layout and subscript syntax
- **Cleaner signatures** — `const Bitboard (&pieces)[2][6]` → `const PositionBitboards& pieces`

## Type Aliases Added

Added to `types/Types.hpp`:

| Alias | Definition | Used for |
|---|---|---|
| `PieceBitboards` | `std::array<Bitboard, Piece::NUM_PIECES>` | One color's 6 piece bitboards |
| `PositionBitboards` | `std::array<PieceBitboards, Color::NUM_COLORS>` | All 12 piece bitboards (2 × 6) |
| `Mailbox` | `std::array<Piece, Square::NUM_SQUARES>` | Square-to-piece lookup table |

## Files Changed

| File | Change |
|---|---|
| `types/Types.hpp` | Added `#include <array>`, `#include "Bitboard.hpp"`, `#include "Square.hpp"`, 3 type aliases |
| `core/HerodotusEngine.hpp` | `pieces` → `PositionBitboards`, `mailbox` → `Mailbox` |
| `core/HerodotusEngine.cpp` | `clearBoard()` → range-for; `printBoardState()` loops → named constants |
| `movegen/KingMoves.hpp` + `.cpp` | Signature: `const PositionBitboards&`, `const Mailbox&` |
| `movegen/PawnMoves.hpp` + `.cpp` | Signature: `const PositionBitboards&`, `const Mailbox&` |
| `hash/ZobristHash.hpp` + `.cpp` | `pieceSquareTable` → `std::array<std::array<uint64_t, 12>, 64>`; `castlingRights` → `std::array<uint64_t, 16>`; `enPassantFiles` → `std::array<uint64_t, 8>`; loops → range-for |
| `utils/BitboardTables.hpp` | Hardcoded `64` → `Square::NUM_SQUARES`; added `#include "types/Types.hpp"` |

## Not Changed (No Raw Arrays)

- `KnightMoves.hpp/.cpp` — takes `Bitboard` by value, no array
- `BishopMoves.hpp/.cpp` — takes `Bitboard` by value, no array
- `RookMoves.hpp/.cpp` — takes `Bitboard` by value, no array
- `QueenMoves.hpp/.cpp` — takes `Bitboard` by value, no array
- `MoveGen.cpp` — `engine.pieces` / `engine.mailbox` pass by reference automatically, subscriptions unchanged

## Build Status

- Compiles with C++20, no warnings on Clang
- Zero overhead — identical assembly to raw arrays
