#ifndef GAME_H
#define GAME_H

#include "Board.h"
#include <string>
#include <chrono>

class Game {
private:
    Board board;
    int score;
    int comboMultiplier; 

    int timeLimit;
    int elapsedBeforeSave;  
    std::chrono::steady_clock::time_point startTime;

    int getRemainingTime() const;

    void processMatches(int &clearedCount, int &recursionCount);

    vector<string> sessionLogs;

    int highScore;
    void loadHighScore();
    void updateHighScore();

public:
    Game();
    void startNewGame();
    void loadGame(const std::string& filename);
    void saveGame(const std::string& filename); 
    void play();
    void showUI();
};

#endif