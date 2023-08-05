#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <cmath>
#include "Board.h"
#include <atomic>



class Player
{   
    private:
        std::string name;
        bool editing = false;
        int score = 0;
        int turn;
        Board board;
    public:
        bool (Board::*selectedAlgorithm)(Tile*, Tile*, int, std::atomic_bool&) = nullptr;
        Player(std::string name, int turn);
        ~Player();
        void onClick(sf::Vector2i pos);
        void buildBoard(unsigned int numRows, unsigned int numCols);
        std::pair<int, int> buildBoard(std::string fileName);
        unsigned int generateBoard(unsigned int numRows = 0, unsigned int numCols = 0, int finishCol = -1);
        Board combineBoard(Player& p2);
        void resetBoard(bool wall = false); // Function to reset the board
        unsigned int editBoard(sf::RenderWindow& window, sf::Font& font, int finishCol = -1);
        std::string getFileName(sf::RenderWindow& window, sf::Font& font);
        void runAlgorithmSelectionWindow(sf::RenderWindow& window, sf::Font& font);
};