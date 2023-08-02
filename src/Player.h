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
        void onClick(sf::Vector2f pos);
        void buildBoard(unsigned int numRows, unsigned int numCols);
        void buildBoard(std::string fileName);
        void editBoard(sf::RenderWindow& window, sf::Font& font);
        void resetBoard(); // Function to reset the board
        std::string getFileName(sf::RenderWindow& winfow, sf::Font& font);
};
