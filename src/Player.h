#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <cmath>
#include "Board.h"
#include <atomic>
#include <functional>



class Player
{   
    private:
        std::string name;
        bool editing = false;
        int score = 0;
        Board board;
        std::vector<sf::Vector2i> path;
        bool algorithmisBFS;
        std::function<bool(Tile*, Tile*, int, std::atomic<bool>&)> selectedAlgorithm; 
    public:
        Player(std::string name);
        ~Player();
        void onClick(sf::Vector2i pos, char state = 'N');
        void buildBoard(unsigned int numRows, unsigned int numCols);
        void buildBoard(std::string fileName);
        unsigned int generateBoard(unsigned int numRows = 0, unsigned int numCols = 0, int finishCol = -1);
        Board combineBoard(Player& p2);
        void resetBoard(bool wall = false); // Function to reset the board
        unsigned int editBoard(sf::RenderWindow& window, sf::Font& font);
        std::string getFileName(sf::RenderWindow& window, sf::Font& font);
        void runAlgorithmSelectionWindow();
        bool (Board::*selectedAlgorithm)(Tile*, Tile*, int, atomic_bool&);

};