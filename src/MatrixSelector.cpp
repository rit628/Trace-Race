#include "MatrixSelector.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>
#include "MusicPlayer.h"
#include <iostream>

MatrixSelector::MatrixSelector(unsigned int maxRows, unsigned int maxCols) : _maxRows(maxRows), _maxCols(maxCols) {
    // Load the font from file and output error message if issue
    if (!_font.loadFromFile("files/font.ttf")) {
        std::cerr << "Error loading font file!" << std::endl;
    }

    liveDimensions.setFont(_font);
    liveDimensions.setCharacterSize(20);
    liveDimensions.setFillColor(sf::Color::Black);
    liveDimensions.setString("Selected Dimensions: 1x1"); // You can set any initial string here

    // Set the position of the live dimensions text at the bottom
    liveDimensions.setPosition(10.f, maxRows * _cellHeight - 30.f);
}

void MatrixSelector::drawGrid(sf::RenderWindow& window, unsigned int& numRows, unsigned int& numCols, bool& isClosed) {
    float cellWidth = window.getSize().x / static_cast<float>(_maxCols);
    float cellHeight = (window.getSize().y - _extraHeight) / static_cast<float>(_maxRows);

    int selectedRow = -1;
    int selectedCol = -1;
    int highlightRow = -1;
    int highlightCol = -1;

    _cellWidth = cellWidth;
    _cellHeight = cellHeight;
      
    liveDimensions.setPosition(130.f, window.getSize().y - 35.f);

    while (window.isOpen()) {
        sf::Event selectorEvent;
        while (window.pollEvent(selectorEvent)) {
            switch (selectorEvent.type) {
                case sf::Event::Closed:
                    window.close();
                    break;
                case Event::MouseMoved:
                    // Update the highlighted cell when the mouse moves
                    highlightRow = static_cast<int>(selectorEvent.mouseMove.y / cellHeight);
                    highlightCol = static_cast<int>(selectorEvent.mouseMove.x / cellWidth);

                    // Update the live dimensions text
                    liveDimensions.setString("Selected Dimensions: " + to_string(highlightRow + 1) + "x" + to_string(highlightCol + 1));
                    break;
                case sf::Event::MouseButtonPressed:
                    if (selectorEvent.mouseButton.button == sf::Mouse::Left) {
                        if (highlightRow >= 0 && highlightCol >= 0) {
                            selectedRow = highlightRow;
                            selectedCol = highlightCol;

                            if (selectedRow < 2 || selectedCol < 2) {
                                std::cout << "Error: Dimensions must be greater than or equal to 3x3." << std::endl;
                                continue;
                            }

                            numRows = selectedRow + 1;
                            numCols = selectedCol + 1;
                        }
                        window.close();
                        isClosed = true;
                    }
                    break;
            }
        }

        window.clear(sf::Color(176, 224, 230));
        for (unsigned int i = 0; i < _maxRows; i++) {
            for (unsigned int j = 0; j < _maxCols; j++) {
                sf::RectangleShape cell(sf::Vector2f(cellWidth, cellHeight));
                cell.setPosition(j * cellWidth, i * cellHeight);
                cell.setOutlineColor(sf::Color::Black);
                cell.setOutlineThickness(1.f);

                if (i <= highlightRow && j <= highlightCol) {
                    sf::RectangleShape highlightCell(sf::Vector2f(cellWidth - 2.f, cellHeight - 2.f));
                    highlightCell.setPosition(j * cellWidth + 1.f, i * cellHeight + 1.f);
                    highlightCell.setFillColor(sf::Color::Blue);
                    highlightCell.setOutlineColor(sf::Color::Black);
                    highlightCell.setOutlineThickness(1.f);
                    window.draw(highlightCell);
                } else {
                    cell.setFillColor(sf::Color(176, 224, 230));
                    window.draw(cell);
                }
            }
        }

        // Draw the live dimensions text
        window.draw(liveDimensions);

        for (unsigned int i = 0; i < _maxRows; i++) {
            for (unsigned int j = 0; j < _maxCols; j++) {
                sf::RectangleShape cellOutline(sf::Vector2f(cellWidth, cellHeight));
                cellOutline.setPosition(j * cellWidth, i * cellHeight);
                cellOutline.setFillColor(sf::Color::Transparent);
                cellOutline.setOutlineColor(sf::Color::Black);
                cellOutline.setOutlineThickness(1.f);
                window.draw(cellOutline);
            }
        }

        window.display();
    }
}
