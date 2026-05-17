// Game.hpp
#pragma once
#include "Board.hpp"

enum class GameResult { Ongoing, WhiteWins, BlackWins, Draw };

class Game {
public:
    Game();

    /**
     * @brief Attempts to apply a UCI-formatted move to the current game state.
     * * This function parses standard UCI strings (e.g., "e2e4", "e7e8q"), validates
     * if the move is pseudo-legal and respects check conditions, and applies it to the board.
     * * @param uci The Universal Chess Interface string representing the move.
     * @return true If the move was legal and successfully applied.
     * @return false If the move was illegal, improperly formatted, or the game is already over.
     */
    bool submitMove(const std::string& uci);


    GameResult  getResult()   const;
    bool isWhiteTurn() const { return whiteTurn; }
    std::string getBoardFen() const;
    const std::vector<std::string>& getMoveHistory() const;

private:
    Board board;
    bool whiteTurn = true;
    GameResult result = GameResult::Ongoing;

    /**
     * @brief Updates the current game state, by getting information from the board class.
     */
    void updateResult();
};