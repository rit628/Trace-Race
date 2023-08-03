#pragma once
#include <SFML/Graphics.hpp>
#include "Tile.h"
#include <vector>
#include <string>
#include <fstream>

class Board : public sf::Drawable
{
    private:
        std::vector<std::vector<Tile*>> tiles;
    public:
        const unsigned int tileDim = 32;
        unsigned int numCols;
        unsigned int numRows;
        void build(unsigned int numCols, unsigned int numRows);
        void build(std::string fileName);
        void resetTiles(bool isWall);
        void reset();
        void draw(sf::RenderTarget& target, sf::RenderStates states) const;
        void updateTile(unsigned int row, unsigned int col);
        void writeToFile(std::string fileName);
};
