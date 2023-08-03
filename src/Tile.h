#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class Tile : public sf::Drawable
{
    private:
        static sf::Texture wall, path;
        std::vector<sf::Sprite> sprites;
    public:
        bool isWall;
        const unsigned int id;
        const sf::Vector2u coords;
        static void loadTextures();
        Tile(unsigned int x, unsigned int y, unsigned int id, bool isWall = true);
        void draw(sf::RenderTarget& target, sf::RenderStates states) const;
        void flip();
};