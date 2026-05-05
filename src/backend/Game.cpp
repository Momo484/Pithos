#include "Game.hpp"

Game::Game() {}

bool Game::submitMove(const std::string& uci) {
    if (result != GameResult::Ongoing) return false;

    Square from = { uci[0] - 'a', uci[1] - '1' };
    Square to   = { uci[2] - 'a', uci[3] - '1' };
    char promo  = uci.size() == 5 ? uci[4] : 0;

    auto legal = board.generateAllLegalMoves(whiteTurn);
    auto it = std::find_if(legal.begin(), legal.end(), [&](const Move& m) {
        return m.getFrom() == from && m.getTo() == to && (promo == 0 || m.getPromotionPiece() == promo);
    });

    if (it == legal.end()) return false;
    board.makeMove(*it);
    whiteTurn = !whiteTurn;
    updateResult();
    return true;
}

GameResult Game::getResult() const { return result; }