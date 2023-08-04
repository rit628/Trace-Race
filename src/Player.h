#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <cmath>
#include "Board.h"
class Player
{   
    private:
        std::string name;
        bool editing = false;
        int score = 0;
        Board board;
    public:
        Player(std::string name);
        ~Player();
        void onClick(sf::Vector2i pos);
        void buildBoard(unsigned int numRows, unsigned int numCols);
        void buildBoard(std::string fileName);
        void resetBoard(bool wall = false); // Function to reset the board
        void editBoard(sf::RenderWindow& window, sf::Font& font);
        std::string getFileName(sf::RenderWindow& window, sf::Font& font);
};