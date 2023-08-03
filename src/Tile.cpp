#include "Tile.h"

using namespace std;
using namespace sf;

Texture Tile::wall;
Texture Tile::path;

void Tile::loadTextures()
{
    wall.loadFromFile("files/images/wall.png");
    path.loadFromFile("files/images/path.png");
}

Tile::Tile(unsigned int x, unsigned int y, unsigned int id, bool isWall) : id(id), coords(x, y)
{
    this->isWall = isWall;
    Sprite base;
    (isWall) ? base.setTexture(wall) : base.setTexture(path);    
    base.setPosition(x,y);
    this->sprites.push_back(base);
}

void Tile::draw(RenderTarget& target, RenderStates states) const
{
    for (auto sprite : sprites)
    {
        target.draw(sprite, states);
    } 
}

void Tile::flip()
{
    isWall = !isWall;
    if (isWall)
    {
        this->sprites.at(0).setTexture(wall);
    }
    else
    {
        this->sprites.at(0).setTexture(path);
    }
}

void Tile::addNeighbor(Direction d, Tile* tile)
{
    Direction opposite = (d == Direction::N) ? Direction::S : Direction::E;
    this->neighbors.emplace(d, tile);
    tile->neighbors.emplace(opposite, this);
}