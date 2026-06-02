#include <iostream>
#include <thread>
#include <chrono>
#include "Game.h"

using namespace std;

int main() {
    Game game;
    int choice = 0;

    while (choice != 3) {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif

        cout << "\033[1;36m"; 
        cout << "==========================================\n";
        cout << "      GEM CASCADE: RECURSIVE EDITION      \n";
        cout << "==========================================\033[0m\n\n";
        
        cout << "  \033[1;33m[1]\033[0m Start New Adventure\n";
        cout << "  \033[1;33m[2]\033[0m Continue Last Game\n";
        cout << "  \033[1;33m[3]\033[0m Exit\n\n";
        
        cout << "  Select an option (1-3): ";
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(1000, '\n');
            continue;
        }

        switch (choice) {
            case 1:
                game.startNewGame();
                break;
            case 2:
                game.loadGame("savegame.txt");
                break;
            case 3:
                cout << "\n\033[1;31mThanks for playing! Goodbye.\033[0m\n";
                this_thread::sleep_for(chrono::seconds(1));
                break;
            default:
                cout << "\n\033[1;31mInvalid choice! Please try again.\033[0m\n";
                this_thread::sleep_for(chrono::seconds(1));
        }
    }

    return 0;
}