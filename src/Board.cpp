#include "Board.h"
#include "Tile.h" 
#include <iostream>
#include <fstream>

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
                temp.at(j)->addNeighbor(temp.at(j-1));
            }
            if (i > 0)
            {
                temp.at(j)->addNeighbor(this->tiles.at(i-1).at(j));
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
    vector<Tile*> wallList;
    unordered_set<Tile*> visited;
    unordered_set<Tile*> added;
    int pathCount = 0;
    int index = dist(gen);
    Tile* currVertex = this->tiles.at(index / numCols).at(index % numCols);
    visited.emplace(currVertex);
    this->paths.push_back(currVertex);
    wallList.insert(wallList.begin(), currVertex->neighbors.begin(), currVertex->neighbors.end());
    added.insert(currVertex->neighbors.begin(), currVertex->neighbors.end());
    while (!wallList.empty())
    {
        pathCount = 0;
        uniform_int_distribution<int> wallSelect(0, wallList.size()-1);
        index = wallSelect(gen);
        currVertex = wallList.at(index);
        wallList.erase(wallList.begin() + index);
        visited.emplace(currVertex);
        for (auto vertex : currVertex->neighbors)
        {
            if (visited.count(vertex) == 1)
            {
                pathCount++;
            }
        }
        if (pathCount > 1)
        {
            continue;
        }
        currVertex->flip();  
        this->paths.push_back(currVertex);
        for (auto vertex : currVertex->neighbors)
        {
            if ((vertex->isWall) && (added.count(vertex) == 0))
            {
                added.emplace(vertex);
                wallList.push_back(vertex);
            }
        }
    }

}