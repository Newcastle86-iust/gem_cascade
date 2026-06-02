#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <iostream>

using namespace std;


const vector<char> GEM_TYPES = {'@', '#', '$', '%', '&'};
const int BOARD_SIZE = 8; 

class Board {
private:
    vector<vector<char>> grid;
    char lastMatchedChar = ' ';
    bool lastMatchRow = true;
    int lastMatchIndex = 0;

public:
    Board(); 
    void initializeBoard(); 
    void displayBoard() const;
    bool swapGems(int r1, int c1, int r2, int c2);

    void deletesamekind(char kind,vector<vector<bool>>& toclear);
    

    bool checkMatches(int &clearedCount); 
    
    void applyGravity();
    bool isDeadlocked(); 
    void shuffleBoard();

    const vector<vector<char>>& getGrid() const;
    void setGrid(const vector<vector<char>>& newGrid);

    void useBomb(int r, int c); 

    void useRocket(bool isRow, int index); 
    bool getHint(int &r1, int &c1, int &r2, int &c2);
    char getLastMatchedChar() const;

    bool isLastMatchRow() const;
    int getLastMatchIndex() const;
    std::string getColorName(char g) const;
};

#endif