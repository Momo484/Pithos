#include "ZobristHash.hpp"
#include <random>
#include <cctype>

bool ZobristHash::initialised = false;
uint64_t ZobristHash::pieceSquareTable[NUM_SQUARES][NUM_PIECES];
uint64_t ZobristHash::whiteToMove;
uint64_t ZobristHash::castlingRights[16];
uint64_t ZobristHash::enPassantFiles[8];

int ZobristHash::getPieceIndex(char symbol) {
    switch (std::tolower(symbol)) {
        case 'p': return std::isupper(symbol) ? 0 : 6;
        case 'n': return std::isupper(symbol) ? 1 : 7;
        case 'b': return std::isupper(symbol) ? 2 : 8;
        case 'r': return std::isupper(symbol) ? 3 : 9;
        case 'q': return std::isupper(symbol) ? 4 : 10;
        case 'k': return std::isupper(symbol) ? 5 : 11;
        default: return -1;
    }
}

void ZobristHash::initialise() {
    if (initialised) return;

    // It is currently fixed for reproducability
    std::mt19937_64 rng(0xDEADBEEF); 
    std::uniform_int_distribution<uint64_t> dist;

    // We give each peice square combination it's own random number for the hash
    // to XOR.
    for (int sq = 0; sq < NUM_SQUARES; sq++) {
        for (int pc = 0; pc < NUM_PIECES; pc++) {
            pieceSquareTable[sq][pc] = dist(rng);
        }
    }

    // similarly we give the other fields their own random number, that is hopefully
    // unique.
    whiteToMove = dist(rng);

    for (int i = 0; i < 16; i++) {
        castlingRights[i] = dist(rng);
    }

    for (int i = 0; i < 8; i++)  {
        enPassantFiles[i] = dist(rng);
    }

    initialised = true;
}

uint64_t ZobristHash::getPieceHash(char symbol, Square sq) {
    int pieceIdx = ZobristHash::getPieceIndex(symbol);
    if (pieceIdx < 0) return 0;

    int squareIdx = sq.y * 8 + sq.x;
    if (squareIdx < 0 || squareIdx >= NUM_SQUARES) return 0;
    return pieceSquareTable[squareIdx][pieceIdx];
}

uint64_t ZobristHash::getCastlingHash(bool whiteKingSide, bool whiteQueenSide, bool blackKingSide, bool blackQueenSide) {
    int idx = 0;
    if (whiteKingSide) idx |= 0x1;
    if (whiteQueenSide) idx |= 0x2;
    if (blackKingSide) idx |= 0x4;
    if (blackQueenSide) idx |= 0x8;
    return castlingRights[idx];
}

uint64_t ZobristHash::getEnPassantHash(Square epTarget) {
    if (epTarget.x < 0 || epTarget.x >= 8) return 0;
    return enPassantFiles[epTarget.x];
}

uint64_t ZobristHash::getToMoveHash(bool isWhite) {
    return isWhite ? whiteToMove : 0;
}
