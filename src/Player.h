#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <cmath>
#include "Board.h"
#include <atomic>
#include <chrono>



class Player
{   
    private:
        std::string name;
        bool editing = false;
        int turn;
        Board board;
    public:
        int score = 0;
        double endTime = 0.0f;
        bool (Board::*selectedAlgorithm)(Tile*, Tile*, int, std::atomic_bool&, std::chrono::time_point<std::chrono::high_resolution_clock>&) = nullptr;
        Player(std::string name, int turn);
        ~Player();
        std::string getDisplayStr();
        void setName(std::string name);
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