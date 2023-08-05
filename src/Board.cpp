#include "Board.h"
#include "Tile.h" 
#include <iostream>
#include <fstream>
#include <utility>
#include <unordered_set>
#include <algorithm>
#include <queue>
#include <stack>
#include <thread>

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

pair<int, int> Board::build(string fileName)
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
    // Gets data for each tile
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
    // Sets start and end tiles
    this->tiles.at(startID / numCols).at(startID % numCols)->flip('S');
    this->tiles.at(finishID / numCols).at(finishID % numCols)->flip('F');
    return {numRows, numCols};
}

void Board::setFinish(unsigned int col)
{
    this->finish = this->tiles.at(0).at(col);
    this->finish->flip('F');
}

Board Board::combine(Board& RHS)
{
    Board combined;
    // Creates a new board with double the height
    combined.build(this->numRows*2, this->numCols);
    // Add top tiles
    for (int i = 0; i < numRows; i++)
    {
        for (int j = 0; j < numCols; j++)
        {
            combined.tiles.at(i).at(j)->isWall = !this->tiles.at(i).at(j)->isWall;
            combined.tiles.at(i).at(j)->flip();
        }
    }
    // Append bottom tiles
    for (int i = 0; i < numRows; i++)
    {
        for (int j = 0; j < numCols; j++)
        {
            combined.tiles.at(i+numRows).at(j)->isWall = !RHS.tiles.at(i).at(j)->isWall;
            combined.tiles.at(i+numRows).at(j)->flip();
        }
    }
    combined.numRows *= 2;
    combined.paths = this->paths;
    combined.paths.insert(RHS.paths.begin(), RHS.paths.end());
    int startID = this->start->id;
    int endID = RHS.start->id;
    // Changes textures on connection tiles
    combined.start = combined.tiles.at(startID / numCols).at(startID % numCols);
    combined.finish = combined.tiles.at((endID /numCols) + numRows).at(endID % numCols);
    combined.start->flip('S');
    combined.finish->flip('F');
    return combined;
}

void Board::resetTiles(bool isWall)
{
    // Resets tiles to initial state without rebuilding board
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
    // Resets tiles to prepare board for rebuilding or deletion
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
    // If the tile is a corner, do nothing
    if (this->tiles.at(row).at(col)->neighbors.size() < 3)
    {
        return;
    }
    
    // If the tile was selected as the start
    if (state == 'S')
    {
        if (this->start != nullptr)
        {
            this->start->flip();
            this->paths.erase(this->start->id);
        }
        this->start = this->tiles.at(row).at(col);
    }
    // If the tile was selected as the finish
    else if (state == 'F')
    {
        if (this->finish != nullptr)
        {
            this->finish->flip();
            this->paths.erase(this->finish->id);
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
    // Randomly generates dimensions if none were assigned
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
    Direction inward;
    Tile* currVertex;
    Tile* targetCell;
    Tile* selected;
    int finishRow = 0;
    int startRow = 0;
    int index = dist(gen);
    // If a finishing point was set by the other player, match it on this board
    if (finishID != -1)
    {
        currVertex = this->tiles.at(0).at(finishID);
        finishRow = 0;
        startRow = numRows - 1;
        currVertex->flip('F');
        this->finish = currVertex;
    }
    // Otherwise pick a random non-edge tile to begin generating the maze
    else
    {
        finishRow = this->numRows-1;
        startRow = 0;
        do
        {
            currVertex = this->tiles.at(index / numCols).at(index % numCols);
            index = dist(gen);
        } 
        while (currVertex->neighbors.size() < 4);
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

    // Populates list of possible finishing tiles
    for (auto tile : this->tiles.at(finishRow))
    {
        if ((tile->id % numCols != 0) && (tile->id % numCols != numCols-1))
        {
            endPoints.push_back(tile);
        }
    }
    
    // Populates list of possible starting tiles
    for (auto tile : this->tiles.at(startRow))
    {
        if ((tile->id % numCols != 0) && (tile->id % numCols != numCols-1))
        {
            startPoints.push_back(tile);
        }
    }

    for (int i = 0; i < numRows; i++)
    {
        if ((i != finishRow) && (i != startRow))
        {
            startPoints.push_back(this->tiles.at(i).at(0));
            startPoints.push_back(this->tiles.at(i).at(numCols-1));
        }
    }

    while (!unexplored.empty())
    {
        // Selects a random vertex from the set of unexplored cells
        uniform_int_distribution<int> wallSelect(0, unexplored.size()-1);
        index = wallSelect(gen);
        currVertex = unexplored.at(index);
        unexplored.erase(unexplored.begin() + index);

        if (currVertex->neighbors.size() < 4)
        {
            continue;            
        }

        currVertex->flip();
        this->paths.emplace(currVertex->id, currVertex);
        
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
        // Randomly selects a neighbor to connect with a path
        uniform_int_distribution<int> pathSelect(0, pathNeighbors.size()-1);
        index = pathSelect(gen);
        selected = pathNeighbors.at(index).first;
        selected->flip();  
        this->paths.emplace(selected->id, selected);
        pathNeighbors.clear();
    }

    inward = (finishRow == 0) ? Direction::S : Direction::N;
    // Randomly generates a finishing tile
    while (this->finish == nullptr)
    {
        uniform_int_distribution<int> finishSelect(0, endPoints.size()-1);
        index = finishSelect(gen);
        // Checks if endpoint is connected to the rest of the maze
        if (!endPoints.at(index)->neighbors.at(inward)->isWall)
        {
            finish = endPoints.at(index);
            finish->flip('F');
        }
        else if (!endPoints.at(index)->neighbors.at(inward)->neighbors.at(inward)->isWall)
        {
            finish = endPoints.at(index);
            finish->flip('F');
            endPoints.at(index)->neighbors.at(inward)->flip();
        }
        
        else
        {
            endPoints.erase(endPoints.begin() + index);
        }
    }

    // Randomly generates starting tile
    while (this->start == nullptr)
    {
        uniform_int_distribution<int> startSelect(0, startPoints.size()-1);
        index = startSelect(gen);
        int id = startPoints.at(index)->id;
        // Computes inward direction based on position in grid
        if (id % numCols == 0)
        {
            inward = Direction::E;
        }
        else if (id % numCols == numCols-1)
        {
            inward = Direction::W;
        }
        else
        {
            inward = (startRow == 0) ? Direction::S : Direction::N;
        }

        // Checks if startpoint is connected to the rest of the maze
        if (!startPoints.at(index)->neighbors.at(inward)->isWall)
        {
            start = startPoints.at(index);
            start->flip('S');
        }
        else if (!startPoints.at(index)->neighbors.at(inward)->neighbors.at(inward)->isWall)
        {
            start = startPoints.at(index);
            start->flip('S');
            startPoints.at(index)->neighbors.at(inward)->flip();
        }
        
        else
        {
            startPoints.erase(startPoints.begin() + index);
        }
    }
}

void Board::clean()
{
    // O(V+E)
    // Cleans the map by removing all disconnected tiles identified via a breadth first search  
    queue<Tile*> q;
    map<int, Tile*> visited;
    Tile* currTile;
    if (this->start != nullptr)
    {
        q.push(this->start);
        visited.emplace(this->start->id, this->start);
    }
    if (this->finish != nullptr)
    {
        q.push(this->finish);
        visited.emplace(this->finish->id, this->finish);
    }

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
    // Computes the intersection between the set of travelable (path) tiles and the set of tiles reachable by from the start or finish
    set_intersection(visited.begin(), visited.end(), this->paths.begin(), this->paths.end(), inserter(connected, connected.end()));
    // Finds the tiles that were marked as travelable (path tiles) but were not reachable from the start or finish
    set_difference(this->paths.begin(), this->paths.end(), visited.begin(), visited.end(), inserter(disconnected, disconnected.end()));
    this->paths = connected;
    // All disconnected tiles are marked as not travelable (and removed from path set)
    for (auto i : disconnected)
    {
        i.second->flip();
    }
    
}

bool Board::isValid()
{
    // O(V+E)
    // Runs a depth first search with source = start and target = finish to check if the map is valid
    stack<Tile*> s;
    map<int, Tile*> visited;
    Tile* currTile;
    if (this->start != nullptr)
    {
        s.push(this->start);
        visited.emplace(this->start->id, this->start);
    }

    while (!s.empty())
    {
        currTile = s.top();
        s.pop();
        for (auto tile : currTile->neighbors)
        {
            if ((visited.count(tile.second->id) == 0) && (!tile.second->isWall))
            {
                s.push(tile.second);
                visited.emplace(tile.second->id, tile.second);
            }
            if (tile.second == this->finish)
            {
                return true;
            }
        }
    }
    return false;
}

bool Board::DFS(Tile* source, Tile* target, int player, atomic_bool& raceEnded, chrono::time_point<chrono::high_resolution_clock>& end)
{
    // O(V+E)
    stack<Tile*> s;
    map<int, Tile*> visited;
    Tile* currTile = source;
    if (currTile == nullptr)
    {
        return false;
    }
    
    s.push(currTile);
    visited.emplace(currTile->id, currTile);
    while ((!s.empty()) && (!raceEnded))
    {
        currTile = s.top();
        s.pop();
        for (auto tile : currTile->neighbors)
        {
            if ((visited.count(tile.second->id) == 0) && (!tile.second->isWall))
            {
                s.push(tile.second);
                visited.emplace(tile.second->id, tile.second);
            }
            if (currTile == target)
            {
                raceEnded = true;
                end = chrono::high_resolution_clock::now();
                return true;
            }
        }
        if (currTile != source)
        {
            currTile->updateTexture(player);
        }
    }
    this_thread::sleep_for(std::chrono::milliseconds(10));
    end = chrono::high_resolution_clock::now();
    return false;
}

bool Board::BFS(Tile* source, Tile* target, int player, atomic_bool& raceEnded, chrono::time_point<chrono::high_resolution_clock>& end)
{
    // O(V+E)
    queue<Tile*> q;
    map<int, Tile*> visited;
    Tile* currTile = source;
    if (currTile == nullptr)
    {
        return false;
    }
    
    q.push(currTile);
    visited.emplace(currTile->id, currTile);
    while ((!q.empty()) && (!raceEnded))
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
            if (currTile == target)
            {
                raceEnded = true;
                end = chrono::high_resolution_clock::now();
                return true;
            }
        }
        if (currTile != source)
        {
            currTile->updateTexture(player);
        }        
    }
    this_thread::sleep_for(std::chrono::milliseconds(10));
    end = chrono::high_resolution_clock::now();
    return false;
}