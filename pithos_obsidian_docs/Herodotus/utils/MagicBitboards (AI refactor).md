---
module: Herodotus
section: utils
source: src/engines/herodotus/utils/MagicBitboards.cpp
status: ai-refactor
---

# MagicBitboards (AI refactor)

## Overview
This note records an AI-assisted refactor of `MagicBitboards.cpp`. It is kept separate from the normal source note so I can review exactly what changed, why, and confirm I understand and agree with it. Nothing here is new science: it is a cleanup of code I had already written (the two magic searches), plus tooling to generate the magic numbers.

## What changed
- **Merged the duplicated searches.** `findRookMagic` and `findBishopMagic` were near-identical. Both now delegate to a single file-local `findMagic(square, mask, generateAttacks, gen)` in an anonymous namespace. The rook/bishop wrappers just pick the relevant mask and the naive generator.
- **Fixed a real bug via the merge.** `findRookMagic` was reading `bishopShifts[...]` instead of `rookShifts[...]` (copy-paste slip). That gave rooks a shift 6 bits too large, squeezing 4096 blocker patterns into 64 buckets, so the search collided forever and never returned. `findMagic` now *derives* the shift itself (`64 - popcount(mask)`), which makes this whole class of mistake impossible.
- **Kept the epoch/stamp collision table** (see design notes below) but tidied it: `uint32_t` stamps, one `table` + one `epoch` allocated per square.
- **Small hygiene:** explicit `#include <vector>`, `std::size_t` loop counters (no more sign-compare warnings), trimmed comments/typos.
- **New tooling:** `utils/generate_magics.cpp` — a standalone executable that searches for every magic, cross-checks each one against the naive slider, and prints paste-ready arrays.
- **CMake:** added the `generate_magics` target. It compiles `MagicBitboards.cpp` directly (does **not** link `HerodotusCore`), so it builds while `movegen/*`, `test_magic.cpp` and `HerodotusEngine.cpp` are still mid-refactor.

Deliberately **not** changed: `std::__popcount` is left as-is; the codebase-wide popcount / `__builtin_popcountll` cleanup is a separate pass.

## Design notes

**One search, parameterised.** `findMagic` takes the square, its relevant mask, and a pointer to the naive slider (`generateRookAttacks` / `generateBishopAttacks`). It:
1. enumerates every subset of the mask with the carry-rippler (`occ = (occ - mask) & mask`) and records the true attacks for each;
2. repeatedly draws a sparse candidate (`rng & rng & rng`), rejects weak ones, and accepts the first that maps every subset to a collision-free index `(occ * magic) >> (64 - bits)`.

Collisions are only a failure when the two subsets have *different* attack sets; subsets that share an attack set may share an index. That is what makes the compression possible at all.

**Shift is derived, not stored.** `bits = popcount(mask)` and `shift = 64 - bits`, so the index lands in `[0, 2^bits)`. This is the single source of truth that ties a magic to the table size the lookup must use.

**Epoch/stamp.** Rather than clearing an `N`-entry table before every candidate, a parallel `epoch[]` records which candidate last wrote each slot. A slot counts as "empty for this candidate" when `epoch[idx] != stamp`. This removes an unconditional O(N) clear per candidate and lets the fail-fast `break` actually pay off (most candidates collide after a handful of subsets). More detail in [[MagicBitboards hpp]].

**Determinism.** The RNG is seeded per square (`base + squareToIndex()`), so each magic is reproducible and independent of the order squares are searched.

## Verification
- `cmake --build build --target generate_magics` builds cleanly; `./build/generate_magics` runs in ~1.2s.
- The program re-derives the attack table for every square from its magic and checks it against the naive slider for **all** subsets — output line `// cross-check: all magics verified against the naive slider`.
- The printed shifts match the canonical relevant-bit counts: rook `52/53/54` (= 12/11/10 bits), bishop `58/59/57/55` (= 6/5/7/9 bits).

## Generated output
Paste `ROOK_MAGICS` / `BISHOP_MAGICS` into `MagicBitboards.hpp`; `ROOK_SHIFTS` / `BISHOP_SHIFTS` are the cross-check (already derivable as `64 - popcount(relevant)`).

```cpp
constexpr std::array<Bitboard, Square::NUM_SQUARES> ROOK_MAGICS = {
    0x0080102040008000ULL,     0x3040084010002000ULL,     0x0d00200041011008ULL,     0x0100081000042102ULL,
    0x1001001008402080ULL,     0x4100040002080100ULL,     0x820008042100c200ULL,     0x0200008844010022ULL,
    0x00048000c00084b0ULL,     0x1004404000201000ULL,     0x0080801000200080ULL,     0x0841001004082100ULL,
    0x1010800400800802ULL,     0x0642000402000810ULL,     0x8100808002000100ULL,     0x0111800080004500ULL,
    0x2000248008400088ULL,     0x1400404000201000ULL,     0x0400808010002000ULL,     0x0018090010002300ULL,
    0x0449010004100800ULL,     0x00d0808004000201ULL,     0x800c440001500802ULL,     0x1064020000408104ULL,
    0x0024800280204003ULL,     0x8800200280400280ULL,     0x0624401200208600ULL,     0x00810a0200102141ULL,
    0x0021040280080080ULL,     0x3804020080800400ULL,     0x1008020400084130ULL,     0x0181004200250484ULL,
    0x0801400022800180ULL,     0x0440100040c02000ULL,     0x8000100080802008ULL,     0x60c0100021000900ULL,
    0x0808004200400400ULL,     0x042e000402001009ULL,     0x8041100204006811ULL,     0x0004800040800100ULL,
    0x0040208040008001ULL,     0x0040400020008080ULL,     0x0004408200120020ULL,     0x0000082012020040ULL,
    0x00c8008004008008ULL,     0x0404040002008080ULL,     0x3800821001440008ULL,     0x206c40c400860009ULL,
    0x0000248001144100ULL,     0x0100824000200180ULL,     0x0040128341220200ULL,     0x4008008408d00180ULL,
    0x0108000400801980ULL,     0x8000020004008080ULL,     0x6210420881100400ULL,     0x1000044401088600ULL,
    0xab80012293028441ULL,     0x1080104004210083ULL,     0x9000084015002001ULL,     0x6040040821001001ULL,
    0x0002002110040802ULL,     0x0001000ea8440001ULL,     0x00020001040800c2ULL,     0x000081024a840222ULL,
};

constexpr std::array<Bitboard, Square::NUM_SQUARES> BISHOP_MAGICS = {
    0x0408203800604082ULL,     0x0904044424002000ULL,     0x411008a204402020ULL,     0x40680a0228400005ULL,
    0x0004104480000010ULL,     0x300c4108400210e0ULL,     0x0001080184600811ULL,     0x1400840401440200ULL,
    0x09000560041c4480ULL,     0x000842042404004aULL,     0x4009842842034000ULL,     0x0020211049000010ULL,
    0x801004042200a005ULL,     0x1080420804048000ULL,     0x4e00060094044200ULL,     0x0202002421080800ULL,
    0x1040020811040082ULL,     0x080850241000c200ULL,     0x0342004104040083ULL,     0x0011041024008000ULL,
    0x2000909c04200008ULL,     0x8091020200420200ULL,     0xe410400121082030ULL,     0xc000810022011008ULL,
    0x02202000c404a400ULL,     0x4090880802022408ULL,     0x2602020020480040ULL,     0xa082080174004008ULL,
    0x90a0488004002000ULL,     0x0000820209004240ULL,     0x94211c00e0424802ULL,     0x2401004420220800ULL,
    0x0810d2200a480820ULL,     0x1088210840100200ULL,     0x8482049000820020ULL,     0x8180020084280080ULL,
    0x0002208400a20020ULL,     0x41c1004200090100ULL,     0x0290410040010440ULL,     0x0204040089182081ULL,
    0x0010842020000880ULL,     0x0002121004010200ULL,     0x0200840402000100ULL,     0x0020044010400204ULL,
    0x1900210202029c20ULL,     0x1051200081801100ULL,     0x4102301400a04104ULL,     0x0204082041110040ULL,
    0xa080820110404080ULL,     0x0010218434600042ULL,     0x4810230409440412ULL,     0x0008080084040010ULL,
    0x0419400410440041ULL,     0x0044102208084212ULL,     0x081220c204004010ULL,     0x0108288124042100ULL,
    0x2094208044202024ULL,     0x88800100aa100220ULL,     0x00008e0100411000ULL,     0x01804020206a0804ULL,
    0x2040300004208200ULL,     0x280038104a101b14ULL,     0x0100c11081010100ULL,     0x004052008200810aULL,
};

constexpr std::array<int, Square::NUM_SQUARES> ROOK_SHIFTS = {
    52,53,53,53,53,53,53,52,53,54,54,54,54,54,54,53,
    53,54,54,54,54,54,54,53,53,54,54,54,54,54,54,53,
    53,54,54,54,54,54,54,53,53,54,54,54,54,54,54,53,
    53,54,54,54,54,54,54,53,52,53,53,53,53,53,53,52,
};

constexpr std::array<int, Square::NUM_SQUARES> BISHOP_SHIFTS = {
    58,59,59,59,59,59,59,58,59,59,59,59,59,59,59,59,
    59,59,57,57,57,57,59,59,59,59,57,55,55,57,59,59,
    59,59,57,55,55,57,59,59,59,59,57,57,57,57,59,59,
    59,59,59,59,59,59,59,59,58,59,59,59,59,59,59,58,
};
```
