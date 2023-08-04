#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <vector>

enum class Direction {N, E, S, W};

class Tile : public sf::Drawable
{
    private:
        static sf::Texture wall, path, start, finish;
        std::vector<sf::Sprite> sprites;
    public:
        bool isWall;
        const unsigned int id;
        const sf::Vector2u coords;
        std::unordered_map<Direction, Tile*> neighbors;
        static void loadTextures();
        Tile(unsigned int x, unsigned int y, unsigned int id, bool isWall = true);
        void draw(sf::RenderTarget& target, sf::RenderStates states) const;
        void flip(char state = 'N');
        void addNeighbor(Direction d, Tile* tile);
};