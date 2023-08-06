#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>
#include "MusicPlayer.h"
#ifndef TRACE_RACE_MATRIXSELECTOR_H
#define TRACE_RACE_MATRIXSELECTOR_H
using namespace std;
using namespace sf;

class MatrixSelector {
private:
    unsigned int _maxRows, _maxCols, _numRows, _numCols;
    float _cellWidth, _cellHeight;
    sf::Text _liveDimensionsText;
    std::string _liveDimensionsStr;
    sf::Text liveDimensions;
    sf::Font _font;
    const float _extraHeight = 50.f;

public:
    // constructor
    MatrixSelector(unsigned int maxRows, unsigned int maxCols);

    // grid building function
    void drawGrid(sf::RenderWindow &window, unsigned int& numRows, unsigned int& numCols, bool& isClosed);

};


#endif //TRACE_RACE_MATRIXSELECTOR_H
