#ifndef TRACE_RACE_BUTTONMAKER_H
#define TRACE_RACE_BUTTONMAKER_H
#include <SFML/Graphics.hpp>
#include "iostream"
using namespace std;
using namespace sf;

class ButtonMaker {
    float _x, _y, _width, _height;
    sf::Rect<float> zone;

public:
    // constructor
    ButtonMaker(float x, float y, float width, float height)
            : _x(x), _y(y), _width(width), _height(height),
              zone(x, y, width, height) {}

    // clickable button bool
    bool isClicked(sf::Vector2i mousePosition);
    // make window scalable
    void updateScale(float scaleX, float scaleY);


};


#endif //TRACE_RACE_BUTTONMAKER_H
