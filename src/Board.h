#pragma once
#include <SFML/Graphics.hpp>
#include "Tile.h"
#include <random>
#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <atomic>
#include <chrono>

class Board : public sf::Drawable
{
    private:
        std::vector<std::vector<Tile*>> tiles;
        std::map<int, Tile*> paths;
    public:
        static std::default_random_engine gen;
        const unsigned int tileDim = 32;
        unsigned int numCols;
        unsigned int numRows;
        Tile* start = nullptr;
        Tile* finish = nullptr;
        ~Board();
        void build(unsigned int numCols, unsigned int numRows);
        std::pair<int, int> build(std::string fileName);
        void setFinish(unsigned int col);
        Board combine(Board& RHS);
        void resetTiles(bool isWall);
        void reset();
        void draw(sf::RenderTarget& target, sf::RenderStates states) const;
        void updateTile(unsigned int row, unsigned int col, char state = 'N');
        void writeToFile(std::string fileName);
        void generate(unsigned int numRows = 0, unsigned int numCols = 0, int finishID = -1);
        void clean();
        bool isValid();
        bool DFS(Tile* source, Tile* target, int player, std::atomic_bool& raceEnded, std::chrono::time_point<std::chrono::high_resolution_clock>& end);
        bool BFS(Tile* source, Tile* target, int player, std::atomic_bool& raceEnded, std::chrono::time_point<std::chrono::high_resolution_clock>& end);
};