#pragma once

#include "types/Bitboard.hpp"
#include "types/Square.hpp"
#include <array>
#include <cstddef>

// Magic bitboards: precomputed attack lookup for the sliding pieces (rooks,
// bishops, queens). For an example of how they are used, see QueenMoves.cpp:
//
//   Bitboard attacks = MagicBitboards::getQueenAttacks(index, occupancy);
//
// `index` is the queen's square and `occupancy` is friendly | enemy — note
// that it is not necessarily "relevant" (only the blockers inside the piece's
// relevant mask can change the result). Attacks on friendlies are filtered
// out later:
//
//   Bitboard captures = attacks & enemy;
//   Bitboard moves = attacks & ~enemy;
//
// This returns every square the queen can engage; the target piece itself is
// retrieved from the mailbox. The lookup should be as quick as indexing into
// an array, skipping the time taken to slide a ray in each direction from the
// queen's position.

namespace MagicBitboards {

// Left shift is positive, right shift is negative.
enum RayDirection {
  NORTH = 8,
  SOUTH = -8,
  EAST = 1,
  WEST = -1,
  NORTH_EAST = 9,
  NORTH_WEST = 7,
  SOUTH_EAST = -7,
  SOUTH_WEST = -9
};

// Casts a ray from the start square in the direction specified, filling the
// bitboard with ones in that direction. Modified to take blockers as a
// parameter: the ray stops when it meets a blocker, including the blocker's
// square (it is a viable attack target) but not moving beyond it. Called with
// blockers == 0ULL, it acts as no blockers.
constexpr Bitboard rayCast(Square start, RayDirection dir, Bitboard blockers) {
  Bitboard result = 0ULL;
  Bitboard curr = start.squareToU64();

  Bitboard wrapMask = 0ULL;
  if (dir == EAST || dir == NORTH_EAST || dir == SOUTH_EAST) {
    wrapMask = FILE_A;
  } else if (dir == WEST || dir == NORTH_WEST || dir == SOUTH_WEST) {
    wrapMask = FILE_H;
  }

  int d = static_cast<int>(dir);
  int absDir = d < 0 ? -d : d;
  // result & blockers is non-zero once a blocker has been hit, at which point
  // the ray stops proceeding.
  while (curr != 0ULL && (result & blockers) == 0ULL) {
    result |= curr;
    curr = (d > 0) ? (curr << absDir) : (curr >> absDir);
    curr &= ~wrapMask;
  }
  // Remove the starting square.
  result &= ~start.squareToU64();
  return result;
}

// For each square, the relevant blocker squares for a rook: the four rays
// radiating from the square, with the terminal (board-edge) square of each ray
// removed. A blocker on a terminal square cannot change the attack set, so it
// is excluded; this is what lets the occupancy patterns be compressed.
constexpr std::array<Bitboard, Square::NUM_SQUARES> generateRookRelevant() {
  std::array<Bitboard, Square::NUM_SQUARES> rookRelevant{};

  for (int i = 0; i < Square::NUM_SQUARES; i++) {
    Square start = Square::fromIndex(i);

    Bitboard rays = rayCast(start, NORTH, 0ULL) | rayCast(start, SOUTH, 0ULL) |
                    rayCast(start, EAST, 0ULL) | rayCast(start, WEST, 0ULL);

    // The board-edge square each ray ends on (excluded from the mask).
    Bitboard endpoints = Square{7, start.file}.squareToU64() |
                         Square{0, start.file}.squareToU64() |
                         Square{start.rank, 7}.squareToU64() |
                         Square{start.rank, 0}.squareToU64();

    rookRelevant[i] = rays & ~endpoints;
  }

  return rookRelevant;
}

// This is the actual array used throughout the code, as a result of the
// generator above.
constexpr auto rookRelevant = generateRookRelevant();

// The bishop counterpart: diagonal rays with the board ring removed.
constexpr std::array<Bitboard, Square::NUM_SQUARES> generateBishopRelevant() {
  std::array<Bitboard, Square::NUM_SQUARES> bishopRelevant{};

  // unlike with rooks, the ring mask does not kill relevant squares, so this is
  // a simple solution to the edge-square cases.
  Bitboard ringMask = FILE_A | FILE_H | RANK_1 | RANK_8;

  for (int i = 0; i < Square::NUM_SQUARES; i++) {
    Square start = Square::fromIndex(i);

    Bitboard rays =
        rayCast(start, NORTH_EAST, 0ULL) | rayCast(start, NORTH_WEST, 0ULL) |
        rayCast(start, SOUTH_EAST, 0ULL) | rayCast(start, SOUTH_WEST, 0ULL);

    bishopRelevant[i] = rays & ~ringMask;
  }

  return bishopRelevant;
}

// This is the actual array used throughout the code, as a result of the
// generator above.
constexpr auto bishopRelevant = generateBishopRelevant();

// The generators and arrays below hold the shift required at each square to
// calculate the final index into the attack tables. They are used later when
// evaluating (((occupancy & relevant[i]) * magic[i]) >> shift[i]).
constexpr std::array<int, Square::NUM_SQUARES> generateRookShifts() {
  std::array<int, Square::NUM_SQUARES> shifts{};
  for (int i = 0; i < 64; i++) {
    shifts[i] = 64 - std::__popcount(rookRelevant[i]);
  }
  return shifts;
}

constexpr auto rookShifts = generateRookShifts();

constexpr std::array<int, Square::NUM_SQUARES> generateBishopShifts() {
  std::array<int, Square::NUM_SQUARES> shifts{};
  for (int i = 0; i < 64; i++) {
    shifts[i] = 64 - std::__popcount(bishopRelevant[i]);
  }
  return shifts;
}

constexpr auto bishopShifts = generateBishopShifts();

constexpr std::size_t ROOK_TABLE_SIZE = 1 << 12; // 12 = max rook relevant bits
constexpr std::size_t BISHOP_TABLE_SIZE = 1
                                          << 9; // 9  = max bishop relevant bits

// Fills the rook and bishop attack tables. Must run once at startup, before
// any of the getters below are called.
void initMagicBitboards();
// Rook attacks for `index` given the full `occupancy` (hash -> table lookup).
Bitboard getRookAttacks(int index, Bitboard occupancy);
// Bishop attacks for `index` given the full `occupancy` (hash -> table lookup).
Bitboard getBishopAttacks(int index, Bitboard occupancy);
// Rook and bishop attacks OR'd together — a queen's moves are their union.
Bitboard getQueenAttacks(int index, Bitboard occupancy);


// Here follows prototypes for naive move generation for both bishops and
// rooks.
Bitboard generateRookAttacks(Square start, Bitboard blockers);
Bitboard generateBishopAttacks(Square start, Bitboard blockers);

// These functions find a suitable magic number for each square; separate
// logic for bishops and rooks.
uint64_t findRookMagic(Square square);
uint64_t findBishopMagic(Square square);

// Magic numbers for speed! Cross-check: all magics verified against the
// naive slider.
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
} // namespace MagicBitboards
