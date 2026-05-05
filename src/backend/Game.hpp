// Game.hpp
#pragma once
#include "Board.hpp"

enum class GameResult { Ongoing, WhiteWins, BlackWins, Draw };

class Game {
public:
    Game();

    // Called by UI / network layer with a move in whatever notation you use
    bool submitMove(const std::string& uci);    // returns false if illegal
    GameResult  getResult()   const;
    bool isWhiteTurn() const { return whiteTurn; }
    std::string getBoardFen() const;
    const std::vector<std::string>& getMoveHistory() const;

private:
    Board board;
    bool whiteTurn = true;
    GameResult result = GameResult::Ongoing;

    void updateResult();          // call after every successful move
};