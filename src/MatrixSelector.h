//
// Created by Catty on 8/2/2023.
//
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

public:
    MatrixSelector(unsigned int maxRows, unsigned int maxCols);

    void drawGrid(sf::RenderWindow &window, unsigned int& numRows, unsigned int& numCols, bool& isClosed);

};


#endif //TRACE_RACE_MATRIXSELECTOR_H
