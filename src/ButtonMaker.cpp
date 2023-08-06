#include "ButtonMaker.h"
// bool for button click
bool ButtonMaker::isClicked(sf::Vector2i mousePosition) {
    return zone.contains(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y));
}
// make button scale with window
void ButtonMaker::updateScale(float scaleX, float scaleY) {
    zone.left = _x * scaleX;
    zone.top = _y * scaleY;
    zone.width = _width * scaleX;
    zone.height = _height * scaleY;
}
