#pragma once

#include "types/Bitboard.hpp"
#include "types/Square.hpp"
#include <array>
#include <vector>

namespace MagicBitboards {

// BishopAttacks
constexpr std::array<Bitboard, Square::NUM_SQUARES> generateBishopAttacks() {
  std::array<Bitboard, Square::NUM_SQUARES> table{};
  auto genDiagAttack = [](int shift, Bitboard mask, bool north,
                          Bitboard startPos) {
    Bitboard attacks = 0ULL;
    Bitboard currPos = startPos;
    while (currPos) {
      if (north) {
        currPos = (currPos << shift) & ~mask;
      } else {
        currPos = (currPos >> shift) & ~mask;
      }
      attacks |= currPos;
    }
    return attacks;
  };

  for (int i = 0; i < Square::NUM_SQUARES; i++) {
    Bitboard attacks = 0ULL;
    Bitboard startPos = 1ULL << i;
    // We are going to include the starting square in the mask and remove at the
    // end

    // north east
    attacks |= genDiagAttack(9, FILE_A, true, startPos);
    // north west
    attacks |= genDiagAttack(7, FILE_H, true, startPos);
    // south east
    attacks |= genDiagAttack(7, FILE_A, false, startPos);
    // south west
    attacks |= genDiagAttack(9, FILE_H, false, startPos);

    table[i] = attacks;
  }

  return table;
}

inline constexpr auto bishopAttacks = generateBishopAttacks();

constexpr std::array<Bitboard, Square::NUM_SQUARES> generateBishopRelevant() {
  std::array<Bitboard, Square::NUM_SQUARES> table{};
  for (int i = 0; i < Square::NUM_SQUARES; i++) {
    int r = i / 8;
    int f = i % 8;
    Bitboard mask = 0ULL;
    // Raycast diagonally, stopping BEFORE the edge
    for (int tr = r + 1, tf = f + 1; tr <= 6 && tf <= 6; tr++, tf++)
      mask |= (1ULL << (tr * 8 + tf));
    for (int tr = r + 1, tf = f - 1; tr <= 6 && tf >= 1; tr++, tf--)
      mask |= (1ULL << (tr * 8 + tf));
    for (int tr = r - 1, tf = f + 1; tr >= 1 && tf <= 6; tr--, tf++)
      mask |= (1ULL << (tr * 8 + tf));
    for (int tr = r - 1, tf = f - 1; tr >= 1 && tf >= 1; tr--, tf--)
      mask |= (1ULL << (tr * 8 + tf));
    table[i] = mask;
  }
  return table;
}

inline constexpr auto bishopRelevant = generateBishopRelevant();
constexpr std::array<Bitboard, Square::NUM_SQUARES> generateRookAttacks() {
  std::array<Bitboard, Square::NUM_SQUARES> table{};
  auto genStraightAttack = [](int shift, Bitboard mask, bool left,
                              Bitboard startPos) {
    Bitboard attacks = 0ULL;
    Bitboard currPos = startPos;
    while (currPos) {
      if (left) {
        currPos = (currPos << shift) & ~mask;
      } else {
        currPos = (currPos >> shift) & ~mask;
      }
      attacks |= currPos;
    }
    return attacks;
  };

  for (int i = 0; i < Square::NUM_SQUARES; i++) {
    Bitboard attacks = 0ULL;
    Bitboard startPos = 1ULL << i;
    // We are going to include the starting square in the mask and remove at the
    // end

    // east
    attacks |= genStraightAttack(1, FILE_A, true, startPos);
    // west
    attacks |= genStraightAttack(1, FILE_H, false, startPos);
    // south
    attacks |= genStraightAttack(8, 0ULL, false, startPos);
    // north
    attacks |= genStraightAttack(8, 0ULL, true, startPos);

    table[i] = attacks;
  }

  return table;
}

inline constexpr auto rookAttacks = generateRookAttacks();

constexpr std::array<Bitboard, Square::NUM_SQUARES> generateRookRelevant() {
  std::array<Bitboard, Square::NUM_SQUARES> table{};
  for (int i = 0; i < Square::NUM_SQUARES; i++) {
    int r = i / 8;
    int f = i % 8;
    Bitboard mask = 0ULL;
    // Raycast straight, stopping BEFORE the edge
    for (int tr = r + 1; tr <= 6; tr++)
      mask |= (1ULL << (tr * 8 + f));
    for (int tr = r - 1; tr >= 1; tr--)
      mask |= (1ULL << (tr * 8 + f));
    for (int tf = f + 1; tf <= 6; tf++)
      mask |= (1ULL << (r * 8 + tf));
    for (int tf = f - 1; tf >= 1; tf--)
      mask |= (1ULL << (r * 8 + tf));
    table[i] = mask;
  }
  return table;
}

inline constexpr auto rookRelevant = generateRookRelevant();

// Magic numbers for perfect hashing
constexpr std::array<Bitboard, 64> bishopMagics = {{
    0x0003100401004A00ULL, 0x0024814802088018ULL, 0x000802104A060258ULL,
    0x2004040485004200ULL, 0x0081114006000001ULL, 0x5902080288240C05ULL,
    0x2088880450044100ULL, 0x4002012208047441ULL, 0x000424605C410208ULL,
    0x02000401020A0214ULL, 0x1100211204004000ULL, 0x0000490411040400ULL,
    0x041A040420000900ULL, 0x7014008804400000ULL, 0x1002008808029004ULL,
    0x402000491C100200ULL, 0x5420040802844806ULL, 0x0444301001380100ULL,
    0x8048029000202823ULL, 0x8404000801433210ULL, 0x0008200308401480ULL,
    0x0000800100514010ULL, 0x080A100048020880ULL, 0x0801002B82809000ULL,
    0x8010048011200200ULL, 0x0001200004189200ULL, 0x5054100001010024ULL,
    0x080B080003014100ULL, 0xA000840084802020ULL, 0x0001010222006100ULL,
    0x0A010C000600A400ULL, 0x0004842201012801ULL, 0x0032200481200840ULL,
    0x048890088A140802ULL, 0x2802005000010904ULL, 0x080C020080080080ULL,
    0x00400802008E4104ULL, 0x0450100040002400ULL, 0x082A208400051443ULL,
    0x1042008E08810841ULL, 0x210C300404209209ULL, 0x0020480824200810ULL,
    0x0082002028000410ULL, 0x0140002018001100ULL, 0x2000100A0090E606ULL,
    0x0840428302100501ULL, 0x0E04882081020400ULL, 0x0024080200403024ULL,
    0x0109080844048C19ULL, 0x0040240108080000ULL, 0x0030004044104644ULL,
    0x0640111242062001ULL, 0x1049100810240230ULL, 0x4050082248C20000ULL,
    0x9204040808610000ULL, 0x0010240800942400ULL, 0x0241010042224005ULL,
    0x0000130108822001ULL, 0x8004800108880410ULL, 0x8015081002050400ULL,
    0x1000002266208200ULL, 0x0800090408908100ULL, 0xC200302018012049ULL,
    0x8440010409005100ULL,
}};

constexpr std::array<Bitboard, 64> rookMagics = {{
    0x2080001880634000ULL, 0x0440082000401000ULL, 0x0280100080200008ULL,
    0x0080100080080004ULL, 0x5080040080080003ULL, 0x8200040200011008ULL,
    0x0880008002000100ULL, 0x0100082140820500ULL, 0x8000800040008021ULL,
    0x0005400040201000ULL, 0x8300802000801000ULL, 0x2002001040220008ULL,
    0x0084800400820800ULL, 0x8413000284010048ULL, 0x130400A104420810ULL,
    0x0041000100008852ULL, 0x0040288000904000ULL, 0x1010004020004000ULL,
    0x0012420022021480ULL, 0x002042000A220010ULL, 0x0268004004020040ULL,
    0x2102808004010200ULL, 0x3020440048011082ULL, 0x0850020000996401ULL,
    0x0060800080204000ULL, 0x1080200080804000ULL, 0x2101001100200044ULL,
    0x0020080080100080ULL, 0x2008080080040080ULL, 0x0094000202000810ULL,
    0x000A810080800200ULL, 0x4C00048A00104304ULL, 0x5000400024800080ULL,
    0x0000200046401001ULL, 0x0008408022001200ULL, 0x0000080080801004ULL,
    0x6CC0040801001100ULL, 0x0004001002020008ULL, 0x2206000402000108ULL,
    0x000C040042003081ULL, 0x8000800040018020ULL, 0x0800200040008080ULL,
    0x0120010040210017ULL, 0x0000080010008080ULL, 0x0018010008110004ULL,
    0x808A000400808002ULL, 0x0006011008440042ULL, 0x0200011060820004ULL,
    0x40024010800D2080ULL, 0x4040008040200280ULL, 0x0000805001200180ULL,
    0x0520220040081200ULL, 0x4000800400080080ULL, 0x0000800400060180ULL,
    0x0000821849100400ULL, 0x000C091444109200ULL, 0x0080010044102481ULL,
    0x0420220100881042ULL, 0x0AA20D0010412001ULL, 0x0242000804204012ULL,
    0x202200881004A002ULL, 0x0002001110681402ULL, 0x884A020108009004ULL,
    0x0140068404411222ULL,
}};
// Lookup tables — built at runtime by initMagicBitboards()
extern std::array<std::vector<Bitboard>, 64> bishopTable;
extern std::array<std::vector<Bitboard>, 64> rookTable;

/// Initialises the magic bitboard lookup tables. Must be called once at engine
/// startup before any move generation is performed.
void initMagicBitboards();

// ───── Inline lookup helpers ─────
// https://rhysre.net/fast-chess-move-generation-with-magic-bitboards.html

inline Bitboard getBishopAttacks(int sq, Bitboard occupancy) {
  Bitboard occ = occupancy & bishopRelevant[sq];
  return bishopTable[sq][(occ * bishopMagics[sq]) >>
                         (64 - std::__popcount(bishopRelevant[sq]))];
}

inline Bitboard getRookAttacks(int sq, Bitboard occupancy) {
  Bitboard occ = occupancy & rookRelevant[sq];
  return rookTable[sq][(occ * rookMagics[sq]) >>
                       (64 - std::__popcount(rookRelevant[sq]))];
}

inline Bitboard getQueenAttacks(int sq, Bitboard occupancy) {
  return getBishopAttacks(sq, occupancy) | getRookAttacks(sq, occupancy);
}

} // namespace MagicBitboards
