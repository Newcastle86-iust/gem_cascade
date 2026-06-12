#include "Game.h"
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <iomanip>

using namespace std;

Game::Game() : score(0), comboMultiplier(1), timeLimit(240), elapsedBeforeSave(0) {}


void Game::processMatches(int &clearedCount, int &recursionCount) {
    char type = board.getLastMatchedChar();
    string colorName = board.getColorName(type);
    
  
    string logEntry = "Step " + to_string(recursionCount) + ": Removing " + to_string(clearedCount) + 
                      " gems (Type '" + type + "', Color: " + colorName + " from " + 
                      (board.isLastMatchRow() ? "Row " : "Col ") + 
                      to_string(board.getLastMatchIndex()) + ")...";
    sessionLogs.push_back(logEntry);

    showUI();
    this_thread::sleep_for(chrono::milliseconds(1500));

    board.applyGravity();
    sessionLogs.push_back(" > Gravity applied, gems falling...");
    
    showUI();
    this_thread::sleep_for(chrono::milliseconds(1500));

    int nextCleared = 0;
    if (board.checkMatches(nextCleared)) {
   
        int earnedScore = clearedCount * 10 * comboMultiplier;
        score += earnedScore;
        
    
        string scoreLog = " -> Match scored: " + to_string(earnedScore) + " (x" + to_string(comboMultiplier) + ")";
        sessionLogs.push_back(scoreLog);
        
        comboMultiplier *= 2;
        sessionLogs.push_back(" -> Chain Reaction! Multiplier increased to x" + to_string(comboMultiplier));
        
        clearedCount = nextCleared;
        recursionCount++;
        
        processMatches(clearedCount, recursionCount);
    } else {
   
        int earnedScore = clearedCount * 10 * comboMultiplier;
        score += earnedScore;
        
        sessionLogs.push_back(" -> Final match scored: " + to_string(earnedScore) + " (x" + to_string(comboMultiplier) + ")");
        sessionLogs.push_back("Reaction complete. Board stable.");
        
        comboMultiplier = 1; 
        
        showUI(); 
        this_thread::sleep_for(chrono::milliseconds(500));
    }
}


int Game::getRemainingTime() const {
    auto now = chrono::steady_clock::now();
    int currentElapsed = chrono::duration_cast<chrono::seconds>(now - startTime).count();
    
    int totalElapsed = elapsedBeforeSave + currentElapsed;
    int remaining = timeLimit - totalElapsed;
    
    return remaining > 0 ? remaining : 0;
}

void Game::showUI() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif





    int rem = getRemainingTime();
    int mins = rem / 60;
    int secs = rem % 60;

    ifstream inFile("highscore.txt");
    if (inFile >> highScore) {
        inFile.close();
    } else {
        highScore = 0;
    }

   const int width = 46;

    cout << "+" << string(width - 2, '-') << "+" << endl;
    cout << "|" << setw(28) << "GEM CASCADE" << setw(18) << "|" << endl;
    cout << "+" << string(width - 2, '-') << "+" << endl;
    

    cout << "| " << left << setw(13) << "High Score" << ": " << right << setw(28) << highScore << " |" << endl;
    cout << "| " << left << setw(13) << "Score"      << ": " << right << setw(28) << score << " |" << endl;
    cout << "| " << left << setw(13) << "Multiplier" << ": " << right << setw(28) << ("x" + to_string(comboMultiplier)) << " |" << endl;
    
   
    stringstream ss;
    ss << setfill('0') << setw(2) << mins << ":" << setw(2) << secs;
    cout << "| " << left << setw(13) << "Time Left"  << ": " << right << setw(28) << ss.str() << " |" << endl;
    
    cout << "+" << string(width - 2, '-') << "+" << endl << endl;

         
    board.displayBoard();

    if(sessionLogs.empty()!=1){
          cout << "\n\n--- GAME LOGS & RECURSION TRACE---\n";
            for(const string& entry : sessionLogs) {
                cout << "> " << entry << endl;
            }
            cout << "-----------------\n\n";
    }
 
}

void Game::play() {
    bool playing = true;
    while (playing) {
     
        if (getRemainingTime() <= 0) {
    showUI();
    cout << "\n===============================\n";
    cout << "   TIME'S UP! GAME OVER! :(\n";
    cout << "   Final Score: " << score << "\n";
    cout << "===============================\n";
    
    updateHighScore(); 

    ofstream outFile("savegame.txt", ios::trunc);
    outFile.close();

    playing = false;
    
    this_thread::sleep_for(chrono::seconds(5));
    continue;
}

  
        if (board.isDeadlocked()) {
            cout << "\nDEADLOCK DETECTED! Shuffling board...\n";
            this_thread::sleep_for(chrono::seconds(2));
            board.shuffleBoard();
        }

        showUI();
        cout << "\nCommands: swap <r1> <c1> <r2> <c2> | bomb(120 points) <r> <c> | rocket(100 points) <row/col> <index> | hint(70 points) | quit\n";
        cout << "Enter command: ";
        
        string cmd;
        cin >> cmd;

        sessionLogs.clear();

        if (cmd == "quit") {
            playing = false;
            saveGame("savegame.txt");
        } 
        else if (cmd == "swap") {
            int r1, c1, r2, c2;
            cin >> r1 >> c1 >> r2 >> c2;
            cout << "\nUser swapped (" << r1 << "," << c1 << ") with (" << r2 << "," << c2 << ")\n";
            
            if (board.swapGems(r1, c1, r2, c2)) {
                int cleared = 0, rec = 1;
                if (board.checkMatches(cleared)) {
                    processMatches(cleared, rec);
                    this_thread::sleep_for(chrono::seconds(2));
                } else {
                    cout << "Invalid move! No match created.\n";
                    board.swapGems(r1, c1, r2, c2); 
                    this_thread::sleep_for(chrono::seconds(1));
                }
            } else {
                cout << "Invalid coordinates!\n";
                this_thread::sleep_for(chrono::seconds(1));
            }
        }
        else if (cmd == "bomb") {
            int r, c;
            cin >> r >> c;
            if (score >= 120) {
                score -= 120;
                cout << "\nUser used BOMB at (" << r << "," << c << ")\n";
                board.useBomb(r, c);
                int cleared = 0, rec = 1;
                if (board.checkMatches(cleared)) processMatches(cleared, rec);
                this_thread::sleep_for(chrono::seconds(2));
            } else {
                cout << "Not enough score (Need 120)!\n";
                this_thread::sleep_for(chrono::seconds(1));
            }
        }
        else if (cmd == "rocket") {
            string type; int index;
            cin >> type >> index;
            if (score >= 100) {
                score -= 100;
                cout << "\nUser used ROCKET on " << type << " " << index << "\n";
                board.useRocket((type == "row"), index);
                int cleared = 0, rec = 1;
                if (board.checkMatches(cleared)) processMatches(cleared, rec);
                this_thread::sleep_for(chrono::seconds(2));
            } else {
                cout << "Not enough score (Need 100)!\n";
                this_thread::sleep_for(chrono::seconds(1));
            }
        }
        else if (cmd == "hint") {
            if (score >= 70) {
                int r1, c1, r2, c2;
                if (board.getHint(r1, c1, r2, c2)) {
                    score -= 70;
                    cout << "Hint: Try swapping (" << r1 << "," << c1 << ") with (" << r2 << "," << c2 << ")\n";
                    this_thread::sleep_for(chrono::milliseconds(1500));
                } else {
                    cout << "No moves available!\n";
                }
            } else {
                cout << "Not enough score (Need 70)!\n";
                this_thread::sleep_for(chrono::seconds(1));
            }
            this_thread::sleep_for(chrono::seconds(2));
        }
        else {
            cout << "\nInvalid Command! Try again.\n";
            this_thread::sleep_for(chrono::milliseconds(800));
        }

        if (playing) {
            saveGame("savegame.txt");
        }
    }
}

void Game::saveGame(const string& filename) {
    ofstream outFile(filename);
    if (outFile.is_open()) {

        auto now = chrono::steady_clock::now();
        int currentElapsed = chrono::duration_cast<chrono::seconds>(now - startTime).count();
        int totalElapsed = elapsedBeforeSave + currentElapsed;

        outFile << score << "\n";
        outFile << totalElapsed << "\n"; 

        const auto& grid = board.getGrid();
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                outFile << grid[i][j];
            }
            outFile << "\n";
        }
        outFile.close();
        this_thread::sleep_for(chrono::seconds(1));
    }
}

void Game::loadGame(const string& filename) {
    ifstream inFile(filename);
    if (inFile.is_open() && inFile.peek() != EOF) {
        inFile >> score;
        inFile >> elapsedBeforeSave; 

        vector<vector<char>> newGrid(8, vector<char>(8));
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                inFile >> newGrid[i][j];
            }
        }
        board.setGrid(newGrid);
        inFile.close();

        startTime = chrono::steady_clock::now(); 
        cout << "Game loaded successfully!\n";
        this_thread::sleep_for(chrono::seconds(5));
        play();
    } else {

        if(inFile.peek() == EOF){
           cout << "No Last Game found !\n";


        }

        else{
          cout << "No save file found!\n";
        }
        this_thread::sleep_for(chrono::seconds(5));
    }
}

void Game::startNewGame() {
    score = 0;
    elapsedBeforeSave = 0;
    startTime = chrono::steady_clock::now(); 
    board = Board();
    

    ofstream clearSave("savegame.txt", ofstream::trunc);
    clearSave.close();
    
    saveGame("savegame.txt");
    
    play();
}


void Game::loadHighScore() {
    ifstream inFile("highscore.txt");
    if (inFile >> highScore) {
        inFile.close();
    } else {
        highScore = 0;
    }
}

void Game::updateHighScore() {
    if (score > highScore) {
        highScore = score;
        ofstream outFile("highscore.txt");
        outFile << highScore;
        outFile.close();
    }
}