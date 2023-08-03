#include "Board.h"
#include "Tile.h" 
#include <iostream>
#include <fstream>
#include <utility>

using namespace std;
using namespace sf;

default_random_engine Board::gen;

void Board::build(unsigned int numRows, unsigned int numCols)
{
    this->reset();
    this->numRows = numRows;
    this->numCols = numCols;
    vector<Tile*> temp;
    int id = 0;
    for (int i = 0; i < numRows; i++)
    {
        temp.clear();
        for (int j = 0; j < numCols; j++)
        {
            temp.push_back(new Tile((tileDim*j), (tileDim*i), id));
            if (j > 0)
            {
                temp.at(j)->addNeighbor(Direction::W, temp.at(j-1));
            }
            if (i > 0)
            {
                temp.at(j)->addNeighbor(Direction::N, this->tiles.at(i-1).at(j));
            }         
            id++;
        }
        this->tiles.push_back(temp);
    }
}

void Board::build(string fileName)
{
    this->reset();
    ifstream file;
    vector<Tile*> temp;
    file.open("files/maps/"+fileName+".cfg");
    string buff;
    file >> buff;    
    this->numRows = stoi(buff.substr(0, buff.find(',')));
    file >> buff;
    this->numCols = stoi(buff);
    unsigned int id;
    bool isWall;
    unsigned int x;
    unsigned int y;
    for (int i = 0; i < numRows; i++)
    {
        temp.clear();
        for (int j = 0; j < numCols; j++)
        {
            file >> buff;
            id = stoi(buff.substr(0, buff.find(',')));
            file >> buff;
            isWall = stoi(buff.substr(0, buff.find(',')));
            file >> buff;
            x = stoi(buff.substr(0, buff.find(',')));
            file >> buff;
            y = stoi(buff);
            temp.push_back(new Tile(x, y, id, isWall));
        }
        this->tiles.push_back(temp);
    }
    
}

void Board::resetTiles(bool isWall)
{
    for (auto& row : tiles)
    {
        for (auto& tile : row)
        {
            tile->isWall = isWall;
            tile->flip();
        }
    }
}

void Board::reset()
{
    for (auto row : tiles)
    {
        for (auto tile : row)
        {
            delete tile;
        }
    }
    this->tiles.clear();
}

void Board::draw(RenderTarget& target, RenderStates states) const
{
    for (auto row : tiles)
    {
        for (auto tile : row)
        {
            target.draw(*tile, states);
        }
    }
}

void Board::updateTile(unsigned int row, unsigned int col)
{
    this->tiles.at(row).at(col)->flip();
}

void Board::writeToFile(string fileName)
{
    ofstream file;
    file.open("files/maps/"+fileName+".cfg");
    file << this->numRows << ", " << this->numCols << '\n';
    for (auto row : this->tiles)
    {
        for (auto tile : row)
        {
            file << tile->id << ", " << tile->isWall << ", " << tile->coords.x << ", " << tile->coords.y << '\n';
        }
        
    }
    
}

void Board::generate(unsigned int numRows, unsigned int numCols)
{
    if (numRows == 0)
    {
        uniform_int_distribution<int> dist(1, 200);
        numRows = dist(gen);
    }
    if (numCols == 0)
    {
        uniform_int_distribution<int> dist(1, 200);
        numCols = dist(gen);
    }
    build(numRows, numCols);
    uniform_int_distribution<int> dist(0, numRows*numCols-1);
    vector<Tile*> unexplored;
    vector<pair<Tile*, Tile*>> pathNeighbors;
    unordered_set<Tile*> added;
    int index = dist(gen);
    int row = index / numCols;
    int col = index % numCols;
    Tile* currVertex = this->tiles.at(row).at(col);
    Tile* targetCell;
    Tile* selected;
    currVertex->flip();
    this->paths.push_back(currVertex);

    for (auto vertex : currVertex->neighbors)
    {
        /// If the current vertex's neighbor at direction X has a neighbor at direction X
        if (vertex.second->neighbors.count(vertex.first) > 0)
        {
            targetCell = vertex.second->neighbors.at(vertex.first);
            added.emplace(targetCell);
            unexplored.push_back(targetCell);
        }
    }

    while (!unexplored.empty())
    {
        // Selects a random vertex from the set of unexplored cells
        uniform_int_distribution<int> wallSelect(0, unexplored.size()-1);
        index = wallSelect(gen);
        currVertex = unexplored.at(index);
        currVertex->flip();
        this->paths.push_back(currVertex);
        unexplored.erase(unexplored.begin() + index);

        // Iterates over all possible targets for traversal
        for (auto vertex : currVertex->neighbors)
        {
            // If the current vertex's neighbor at direction X has a neighbor at direction X, add it as a target destination
            if (vertex.second->neighbors.count(vertex.first) > 0)
            {
                targetCell = vertex.second->neighbors.at(vertex.first);
                // If target is a path, add it to the list of possible paths to connect to
                if (!targetCell->isWall)
                {
                    pathNeighbors.push_back({vertex.second, targetCell});
                }
                // Otherwise, if target has not been identified, mark it as unexplored
                else if (added.count(targetCell) == 0)
                {
                    added.emplace(targetCell);
                    unexplored.push_back(targetCell);
                }
            }
        }
        uniform_int_distribution<int> pathSelect(0, pathNeighbors.size()-1);
        index = pathSelect(gen);
        selected = pathNeighbors.at(index).first;
        selected->flip();  
        this->paths.push_back(selected);
        pathNeighbors.clear();
    }

}