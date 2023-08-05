#include "Tile.h"

using namespace std;
using namespace sf;

Texture Tile::wall;
Texture Tile::path;
Texture Tile::start;
Texture Tile::finish;
Texture Tile::p1;
Texture Tile::p2;
Texture Tile::overlap;

void Tile::loadTextures()
{
    wall.loadFromFile("files/images/wall.png");
    path.loadFromFile("files/images/path.png");
    start.loadFromFile("files/images/start.png");
    finish.loadFromFile("files/images/finish.png");
    p1.loadFromFile("files/images/p1.png");
    p2.loadFromFile("files/images/p2.png");
    overlap.loadFromFile("files/images/overlap.png");
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

void Tile::flip(char state)
{
    isWall = !isWall;
    if (state == 'S')
    {
        this->sprites.at(0).setTexture(start);
        isWall = false;
    }
    else if (state == 'F')
    {
        this->sprites.at(0).setTexture(finish);
        isWall = false;
    }   
    else if (isWall)
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

void Tile::updateTexture(int player)
{
    if (player == 1)
    {
        if (this->sprites.at(0).getTexture() == &Tile::p2)
        {
            this->sprites.at(0).setTexture(overlap);
        }
        else
        {
            this->sprites.at(0).setTexture(p1);
        }
    }
    else
    {
        if (this->sprites.at(0).getTexture() == &Tile::p1)
        {
            this->sprites.at(0).setTexture(overlap);
        }
        else
        {
            this->sprites.at(0).setTexture(p2);
        }
    }
}