//
// Created by Catty on 8/2/2023.
//

#include "MatrixSelector.h"

MatrixSelector::MatrixSelector(unsigned int maxRows, unsigned int maxCols) :_maxRows(maxRows), _maxCols(maxCols){
}

void MatrixSelector::drawGrid(sf::RenderWindow &window, unsigned int& numRows, unsigned int& numCols, bool& isClosed) {
// Matrix Selector Section

    _cellWidth = window.getSize().x / static_cast<float>(_maxCols);
    _cellHeight = window.getSize().y / static_cast<float>(_maxRows);
    while (window.isOpen()) {
        Event selectorEvent;
        while (window.pollEvent(selectorEvent)) {
            switch (selectorEvent.type) {
                case Event::Closed:
                    window.close();
                    break;

                case Event::MouseButtonPressed:
                    // when size is selected, close menu
                    if (selectorEvent.mouseButton.button == Mouse::Left) {
                        numRows = static_cast<int>(selectorEvent.mouseButton.y / _cellHeight) + 1;
                        numCols = static_cast<int>(selectorEvent.mouseButton.x / _cellWidth) + 1;
                        isClosed = true;
                        window.close();
                    }
                    break;
            }
        }
        Vector2i mousePositionMatrixSelector = Mouse::getPosition(window);
        window.clear(Color::White);
        // create grid
        for (unsigned int i = 0; i < _maxRows; i++) {
            for (unsigned int j = 0; j < _maxCols; j++) {
                RectangleShape cell(Vector2f(_cellWidth, _cellHeight));
                cell.setPosition(j * _cellWidth, i * _cellHeight);
                cell.setOutlineColor(Color::Black);
                cell.setOutlineThickness(1.f);

                if (i * _cellHeight <= mousePositionMatrixSelector.y &&
                    j * _cellWidth <= mousePositionMatrixSelector.x &&
                    (i + 1) * _cellHeight > mousePositionMatrixSelector.y &&
                    (j + 1) * _cellWidth > mousePositionMatrixSelector.x) {
                    // highlight cells over entire matrix as mouse rolls over
                    for (int r = 0; r <= i; r++) {
                        for (int c = 0; c <= j; c++) {
                            RectangleShape highlightCell(Vector2f(_cellWidth, _cellHeight));
                            highlightCell.setPosition(c * _cellWidth, r * _cellHeight);
                            highlightCell.setFillColor(Color::Blue);
                            highlightCell.setOutlineColor(Color::Black);
                            highlightCell.setOutlineThickness(1.f);
                            window.draw(highlightCell);
                        }
                    }
                }
                else {
                    cell.setFillColor(Color::White);
                }
                window.draw(cell);
            }
        }
        window.display();
    }
};
