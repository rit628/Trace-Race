#include "Board.h"
#include "Tile.h" 
#include <iostream>
#include <fstream>
#include <utility>
#include <unordered_set>
#include <algorithm>
#include <queue>

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
            // Adds West Neighbor
            if (j > 0)
            {
                temp.at(j)->addNeighbor(Direction::W, temp.at(j-1));
            }
            // Adds North Neighbor
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
    // Builds board from save file
    this->reset();
    ifstream file;
    vector<Tile*> temp;
    file.open("files/maps/"+fileName+".cfg");
    string buff;
    file >> buff;    
    this->numRows = stoi(buff.substr(0, buff.find(',')));
    file >> buff;
    this->numCols = stoi(buff.substr(0, buff.find(',')));
    file >> buff;
    unsigned int startID = stoi(buff.substr(0, buff.find(',')));
    file >> buff;
    unsigned int finishID = stoi(buff);
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
            this->paths.emplace(id, temp.at(j));
        }
        this->tiles.push_back(temp);
    }
    this->tiles.at(startID / numCols).at(startID % numCols)->flip('S');
    this->tiles.at(finishID / numCols).at(finishID % numCols)->flip('F');
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
    this->start = nullptr;
    this->finish = nullptr;
    this->paths.clear();
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
    this->start = nullptr;
    this->finish = nullptr;
    this->paths.clear();
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

void Board::updateTile(unsigned int row, unsigned int col, char state)
{
    // If the tile was selected as the start
    if (state == 'S')
    {
        if (this->start != nullptr)
        {
            this->start->flip();
        }
        this->start = this->tiles.at(row).at(col);
        cout << "set start at: " << this->start->id << endl;
    }
    // If the tile was selected as the finish
    else if (state == 'F')
    {
        if (this->finish != nullptr)
        {
            this->finish->flip();
        }
        this->finish = this->tiles.at(row).at(col);
    }
    // Change tile from wall to path or vice versa
    this->tiles.at(row).at(col)->flip(state);
    if (this->tiles.at(row).at(col)->isWall)
    {
        this->paths.erase(this->tiles.at(row).at(col)->id);
    }
    else
    {
        this->paths.emplace(this->tiles.at(row).at(col)->id, this->tiles.at(row).at(col));
    } 
}

void Board::writeToFile(string fileName)
{
    /* Map data schema
    1       Rows, Columns, Start Tile ID, Finish Tile ID
    2-RxC   ID, Wall/Path, x, y
    */
    ofstream file;
    file.open("files/maps/"+fileName+".cfg");
    file << this->numRows << ", " << this->numCols << ", " << this->start->id << ", " << this->finish->id << '\n';
    for (auto row : this->tiles)
    {
        for (auto tile : row)
        {
            file << tile->id << ", " << tile->isWall << ", " << tile->coords.x << ", " << tile->coords.y << '\n';
        }
        
    }
    
}

void Board::generate(unsigned int numRows, unsigned int numCols, int finishID)
{
    if (numRows == 0)
    {
        uniform_int_distribution<int> dist(3, 200);
        numRows = dist(gen);
    }
    if (numCols == 0)
    {
        uniform_int_distribution<int> dist(3, 200);
        numCols = dist(gen);
    }
    build(numRows, numCols);
    uniform_int_distribution<int> dist(0, numRows*numCols-1);
    vector<Tile*> unexplored;
    vector<Tile*> startPoints;
    vector<Tile*> endPoints;
    vector<pair<Tile*, Tile*>> pathNeighbors;
    unordered_set<Tile*> added;
    Tile* currVertex;
    Tile* targetCell;
    Tile* selected;
    int illegalRow = -1;
    int index = dist(gen);
    int row = index / numCols;
    int col = index % numCols;
    if (finishID != -1)
    {
        currVertex = this->tiles.at(finishID / numCols).at(finishID % numCols);
        illegalRow = finishID / numCols;
        currVertex->flip('F');
        this->finish = currVertex;
    }
    else
    {
        currVertex = this->tiles.at(row).at(col);
        currVertex->flip();        
    }
    
    this->paths.emplace(currVertex->id, currVertex);
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
        this->paths.emplace(currVertex->id, currVertex);
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
        this->paths.emplace(selected->id, selected);
        pathNeighbors.clear();
        if ((currVertex->id / numCols) != illegalRow)
        {
            if ((currVertex->id / numCols == 0) || (currVertex->id / numCols == numRows-1))
            {
                endPoints.push_back(currVertex);
            }
            else if (currVertex->neighbors.size() < 4)
            {
                startPoints.push_back(currVertex);
            }
        }
        
        if ((selected->id / numCols) != illegalRow)
        {
            if ((selected->id / numCols == 0) || (selected->id / numCols == numRows-1))
            {
                endPoints.push_back(selected);
            }
            else if (selected->neighbors.size() < 4)
            {
                startPoints.push_back(selected);
            }
        }        
    }
    Direction inward;
    if (this->finish == nullptr)
    {
        if (endPoints.size() == 0)
        {
            uniform_int_distribution<int> finishSelect(0, 1);
            int row = (finishSelect(gen) == 0) ? 0 : numRows-1;
            inward = (row == 0) ? Direction::S : Direction::N;
            for (int i = 0; i < numCols; i++)
            {
                if (!this->tiles.at(row).at(i)->neighbors.at(inward)->isWall)
                {
                    endPoints.push_back(this->tiles.at(row).at(i));
                }
                
            }
            
        }
        
        uniform_int_distribution<int> finishSelect(0, endPoints.size()-1);
        index = finishSelect(gen);
        this->finish = endPoints.at(index);
        this->finish->flip('F');
        illegalRow = this->finish->id / numCols;
        for (int i = 0; i < endPoints.size(); i++)
        {
            if ((endPoints.at(i)->id / numCols) != illegalRow)
            {
                startPoints.push_back(endPoints.at(i));
            }
            
        }
        
    }
    
    if (startPoints.size() == 0)
    {
        int row = (illegalRow == 0) ? numRows-1 : 0;
        inward = (row == 0) ? Direction::S : Direction::N;
        for (int i = 0; i < numCols; i++)
        {
            startPoints.push_back(this->tiles.at(row).at(i));
        }
    }
    do
    {
        uniform_int_distribution<int> startSelect(0, startPoints.size()-1);
        index = startSelect(gen);
    } 
    while ((startPoints.at(index)->isWall) && (startPoints.at(index)->neighbors.at(inward)->isWall));
    
    
    this->start = startPoints.at(index);
    this->start->flip('S');


}

void Board::makeConnected()
{
    // Runs a breadth first search starting at the "start" labeled tile to determine all nodes that can be reached from it
    queue<Tile*> q;
    map<int, Tile*> visited;
    Tile* currTile = this->start;
    q.push(currTile);
    visited.emplace(currTile->id, currTile);
    while (!q.empty())
    {
        currTile = q.front();
        q.pop();
        for (auto tile : currTile->neighbors)
        {
            if ((visited.count(tile.second->id) == 0) && (!tile.second->isWall))
            {
                q.push(tile.second);
                visited.emplace(tile.second->id, tile.second);
            }
            
        }
        
    }
    map<int, Tile*> connected;
    map<int, Tile*> disconnected;
    // Computes the intersection between the set of travelable nodes and the set of nodes reachable by from the start
    set_intersection(visited.begin(), visited.end(), this->paths.begin(), this->paths.end(), inserter(connected, connected.end()));
    // Finds the nodes that were marked as travelable but were not reachable from the start
    set_difference(this->paths.begin(), this->paths.end(), visited.begin(), visited.end(), inserter(disconnected, disconnected.end()));
    this->paths = connected;
    cout << "Disconnected Nodes" << endl;
    // All disconnected nodes are marked as not travelable
    for (auto i : disconnected)
    {
        i.second->flip();
        cout << i.first << endl;
    }
    
}