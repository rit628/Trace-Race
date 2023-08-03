#pragma once
#include <SFML/Graphics.hpp>
#include "Tile.h"
#include <random>
#include <unordered_set>
#include <vector>
#include <string>
#include <fstream>

class Board : public sf::Drawable
{
    private:
        static std::default_random_engine gen;
        std::vector<std::vector<Tile*>> tiles;
        std::vector<Tile*> paths;
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
        void generate(unsigned int numRows = 0, unsigned int numCols = 0);
};
