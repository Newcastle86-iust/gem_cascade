#include "Board.h"
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>

Board::Board() {
    grid.resize(BOARD_SIZE, vector<char>(BOARD_SIZE, ' '));
    srand(time(0));
    initializeBoard();
}

void Board::initializeBoard() {
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            char newGem;
            bool validGem = false;
            while (!validGem) {
                newGem = GEM_TYPES[rand() % GEM_TYPES.size()];
                if ((i >= 2 && grid[i-1][j] == newGem && grid[i-2][j] == newGem) ||
                    (j >= 2 && grid[i][j-1] == newGem && grid[i][j-2] == newGem)) {
                    continue;
                }
                validGem = true;
            }
            grid[i][j] = newGem;
        }
    }
}

void Board::deletesamekind(char kind,vector<vector<bool>>& toclear) {
     for(int i=0;i<BOARD_SIZE;++i){
        for(int j=0;j<BOARD_SIZE;j++){
            if(grid[i][j]==kind){
                toclear[i][j]=true;
            }
        }
     }

}

void Board::displayBoard() const {
    cout << "\n        "; 
    for (int j = 0; j < BOARD_SIZE; ++j) cout << "   " << j << "    "; 
    cout << "\n      +";
    for (int j = 0; j < BOARD_SIZE; ++j) cout << "-------+";
    cout << "\n";

    for (int i = 0; i < BOARD_SIZE; ++i) {
        cout << "   " << i << "  |";
        for (int j = 0; j < BOARD_SIZE; ++j) {
            char g = grid[i][j];
            cout << "   ";
            if (g == '@') cout << "\033[1;31m" << g << "\033[0m";
            else if (g == '#') cout << "\033[1;32m" << g << "\033[0m";
            else if (g == '$') cout << "\033[1;34m" << g << "\033[0m";
            else if (g == '%') cout << "\033[1;33m" << g << "\033[0m";
            else if (g == '&') cout << "\033[1;35m" << g << "\033[0m";
            else cout << g;
            cout << "   |";
        }
        cout << "\n      +";
        for (int j = 0; j < BOARD_SIZE; ++j) cout << "-------+";
        cout << "\n";
    }
    cout << endl;
}

bool Board::swapGems(int r1, int c1, int r2, int c2) {
    if (abs(r1 - r2) + abs(c1 - c2) != 1) return false;
    swap(grid[r1][c1], grid[r2][c2]);
    return true;
}

bool Board::checkMatches(int &clearedCount) {
    bool matchFound = false;
    clearedCount = 0;
    vector<vector<bool>> toClear(BOARD_SIZE, vector<bool>(BOARD_SIZE, false));

    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j <= BOARD_SIZE - 3; ) {
            int len = 1;
            while (j + len < BOARD_SIZE && grid[i][j + len] == grid[i][j] && grid[i][j] != ' ') len++;
            if (len >= 3) {
                matchFound = true;
                lastMatchedChar = grid[i][j];
                lastMatchRow = true;
                lastMatchIndex = i;
                for (int k = 0; k < len; ++k) toClear[i][j + k] = true;
                
                if(len==4){
                    for (int k = 0; k < BOARD_SIZE; ++k) toClear[i][k] = true;
                }

                if(len==5){
                    deletesamekind(lastMatchedChar,toClear);
                }
            }
            j += (len > 1) ? len : 1;
        }
    }

    for (int j = 0; j < BOARD_SIZE; ++j) {
        for (int i = 0; i <= BOARD_SIZE - 3; ) {
            int len = 1;
            while (i + len < BOARD_SIZE && grid[i + len][j] == grid[i][j] && grid[i][j] != ' ') len++;
            if (len >= 3) {
                matchFound = true;
                lastMatchedChar = grid[i][j];
                lastMatchRow = false;
                lastMatchIndex = j;
                for (int k = 0; k < len; ++k) toClear[i + k][j] = true;

                if(len==4){
                    for (int k = 0; k < BOARD_SIZE; ++k) toClear[k][j] = true;
                }

                if(len==5){
                         deletesamekind(lastMatchedChar,toClear);
                }
            }
            i += (len > 1) ? len : 1;
        }
    }

    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (toClear[i][j]) { grid[i][j] = ' '; clearedCount++; }
        }
    }
    return matchFound;
}

void Board::applyGravity() {
    for (int j = 0; j < BOARD_SIZE; ++j) {
        int emptyRow = BOARD_SIZE - 1;
        for (int i = BOARD_SIZE - 1; i >= 0; --i) {
            if (grid[i][j] != ' ') {
                swap(grid[i][j], grid[emptyRow][j]);
                emptyRow--;
            }
        }
    }
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (grid[i][j] == ' ') grid[i][j] = GEM_TYPES[rand() % GEM_TYPES.size()];
        }
    }
}


const vector<vector<char>>& Board::getGrid() const {
    return grid;
}

void Board::setGrid(const vector<vector<char>>& newGrid) {
    grid = newGrid;
}

void Board::useBomb(int r, int c) {

    for (int i = max(0, r - 1); i <= min(BOARD_SIZE - 1, r + 1); ++i) {
        for (int j = max(0, c - 1); j <= min(BOARD_SIZE - 1, c + 1); ++j) {
            grid[i][j] = ' ';
        }
    }
    applyGravity();
}

void Board::useRocket(bool isRow, int index) {

    if (index < 0 || index >= BOARD_SIZE) return;
    
    if (isRow) {
        for (int j = 0; j < BOARD_SIZE; ++j) grid[index][j] = ' ';
    } else {
        for (int i = 0; i < BOARD_SIZE; ++i) grid[i][index] = ' ';
    }
    applyGravity();
}



bool Board::isDeadlocked() {


    vector<vector<char>> originalGrid = grid;
    int dummyCount = 0;

    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
      

            if (j < BOARD_SIZE - 1) {
                swap(grid[i][j], grid[i][j+1]);
                if (checkMatches(dummyCount)) {
                    grid = originalGrid; 
                    return false; 
                }
                grid = originalGrid; 
            }
      
            if (i < BOARD_SIZE - 1) {
                swap(grid[i][j], grid[i+1][j]);
                if (checkMatches(dummyCount)) {
                    grid = originalGrid;
                    return false;
                }
                grid = originalGrid; 
            }
        }
    }
    return true;
}

void Board::shuffleBoard() {

    do {
        initializeBoard(); 
    } while (isDeadlocked());
}



bool Board::getHint(int &bestR1, int &bestC1, int &bestR2, int &bestC2) {
    int maxCleared = -1;
    vector<vector<char>> originalGrid = grid;

    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            
          
            if (j < BOARD_SIZE - 1) {
                swap(grid[i][j], grid[i][j+1]);
                int clearedCount = 0;
                if (checkMatches(clearedCount) && clearedCount > maxCleared) {
                    maxCleared = clearedCount;
                    bestR1 = i; bestC1 = j;
                    bestR2 = i; bestC2 = j+1; 
                }
                grid = originalGrid; 
            }
            
       
            if (i < BOARD_SIZE - 1) {
                swap(grid[i][j], grid[i+1][j]);
                int clearedCount = 0;
                if (checkMatches(clearedCount) && clearedCount > maxCleared) {
                    maxCleared = clearedCount;
                    bestR1 = i; bestC1 = j;
                    bestR2 = i + 1; bestC2 = j; 
                }
                grid = originalGrid; 
            }
        }
    }

    return (maxCleared > 0); 
}


char Board::getLastMatchedChar() const { return lastMatchedChar; }
bool Board::isLastMatchRow() const { return lastMatchRow; }
int Board::getLastMatchIndex() const { return lastMatchIndex; }
string Board::getColorName(char g) const {
    if (g == '@') return "red";
    if (g == '#') return "green";
    if (g == '$') return "blue";
    if (g == '%') return "yellow";
    if (g == '&') return "purple";
    return "unknown";
}