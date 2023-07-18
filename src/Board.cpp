#include "Board.h"

using namespace std;
using namespace sf;

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
    for (int i = 0; i < numCols; i++)
    {
        temp.clear();
        for (int j = 0; j < numRows; j++)
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