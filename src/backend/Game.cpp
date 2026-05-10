#include "Game.hpp"

Game::Game() {}

void Game::updateResult() {
    // here we check what state the game is currently at
    // the board is responsible for determining if is a stalemate, 50 move rule and 3 move rule or a checkmate

    auto legal = board.generateAllLegalMoves(whiteTurn);
    bool kingInCheck = board.isKingChecked(whiteTurn);
    if (legal.empty()) {
        if (kingInCheck) {
            result = whiteTurn ? GameResult::BlackWins : GameResult::WhiteWins;
        } else {
            // stale mate situation
            result = GameResult::Draw;
        }
    }

    // we leave the 50 move and 3 move ruls for later.
}

bool Game::submitMove(const std::string& uci) {
    if (result != GameResult::Ongoing) return false;

    // uci is purely, square to square, and promo peice appending if it wants it
    Square from = { uci[0] - 'a', uci[1] - '1' };
    Square to   = { uci[2] - 'a', uci[3] - '1' };
    char promo  = uci.size() == 5 ? uci[4] : 0;

    // get all of the legal moves for the white pieces
    // this is kinda slow and over built rn, ill fix it later.
    auto legal = board.generateAllLegalMoves(whiteTurn);

    // Find any move that matches the start and end square, this should be unique, as the start square
    // is unique, it also has to match the promotion tho if the uci is a promotion asking move.
    auto it = std::find_if(legal.begin(), legal.end(), [&](const Move& m) {
        return m.getFrom() == from && m.getTo() == to && (promo == 0 || m.getPromotionPiece() == promo);
    });

    // if that move has not been found then we return false.
    if (it == legal.end()) return false;
    // otherwise we make the move. it is guarenteed to be legal.
    board.makeMove(*it);
    whiteTurn = !whiteTurn;
    updateResult();
    return true;
}

GameResult Game::getResult() const { return result; }