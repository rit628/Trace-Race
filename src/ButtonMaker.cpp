//
// Created by Catty on 8/3/2023.
//

#include "ButtonMaker.h"

bool ButtonMaker::isClicked(sf::Vector2i mousePosition) {
    return zone.contains(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y));
}

void ButtonMaker::updateScale(float scaleX, float scaleY) {
    zone.left = _x * scaleX;
    zone.top = _y * scaleY;
    zone.width = _width * scaleX;
    zone.height = _height * scaleY;
}
