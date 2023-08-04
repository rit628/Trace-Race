#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include "Player.h"
#include "iostream"

using namespace std;
using namespace sf;

string querySelectorDEBUG(RenderWindow& window, Font& font, string query);

int main(int argc, char const* argv[])
{
    Tile::loadTextures();
    Font font;
    font.loadFromFile("files/font.ttf");
    unsigned int monitorWidth = VideoMode::getDesktopMode().width;
    unsigned int monitorHeight = VideoMode::getDesktopMode().height;
    unsigned int numRows = 10;
    unsigned int numCols = 10;
    string fileName = "map";
    string dims;

    sf::Texture mainMenuTexture;
    mainMenuTexture.loadFromFile("files/images/main_menu.png");
    RenderWindow window(VideoMode(mainMenuTexture.getSize().x, mainMenuTexture.getSize().y), "Trace Race", Style::Default);

    sf::Vector2i screenCenter(sf::VideoMode::getDesktopMode().width / 2, sf::VideoMode::getDesktopMode().height / 2);
    sf::Vector2i windowPosition(screenCenter.x - window.getSize().x / 2, screenCenter.y - window.getSize().y / 2);
    window.setPosition(windowPosition);

    window.setFramerateLimit(360);

    float scaleX = (float)window.getSize().x / mainMenuTexture.getSize().x;
    float scaleY = (float)window.getSize().y / mainMenuTexture.getSize().y;
    sf::Sprite mainMenuSprite;
    mainMenuSprite.setTexture(mainMenuTexture);
    mainMenuSprite.setScale(scaleX, scaleY);

    float newGameX = 0.0f;
    float newGameY = 222.0f;
    float newGameWidth = 315.0f;
    float newGameHeight = 52.0f;

    float scaledNewGameX = newGameX * scaleX;
    float scaledNewGameY = newGameY * scaleY;
    float scaledNewGameWidth = newGameWidth * scaleX;
    float scaledNewGameHeight = newGameHeight * scaleY;

    sf::Rect<float> newGameZone(scaledNewGameX, scaledNewGameY, scaledNewGameWidth, scaledNewGameHeight);

    Player* p1 = new Player("Debug");

    Text liveDimensions("", font, 16);
    liveDimensions.setFillColor(Color::Black);


    while (window.isOpen())
    {
        Vector2i mousePosition = Mouse::getPosition(window);
        bool isMatrixSelectorClosed = false;
        bool isManualInputClosed = false;

        window.clear(Color::Blue);

        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Resized)
            {
                sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                window.setView(sf::View(visibleArea));

                scaleX = (float)event.size.width / mainMenuTexture.getSize().x;
                scaleY = (float)event.size.height / mainMenuTexture.getSize().y;

                mainMenuSprite.setScale(scaleX, scaleY);

                newGameZone.left = newGameX * scaleX;
                newGameZone.top = newGameY * scaleY;
                newGameZone.width = newGameWidth * scaleX;
                newGameZone.height = newGameHeight * scaleY;
            }
            switch (event.type)
            {
                case Event::Closed:
                    window.close();
                    break;
                case Event::MouseButtonPressed:
                    if (newGameZone.contains(event.mouseButton.x, event.mouseButton.y))
                    {
                        RenderWindow selectorType(VideoMode(600, 600), "Choose Dimensional Input", Style::Close);
                        selectorType.setPosition(windowPosition);
                        selectorType.setFramerateLimit(360);
                        Text inputChoice("f- Matrix Input\n"
                                         "d- Manual Input \n",
                                         font, 24);
                        inputChoice.setFillColor(Color::Black);
                        Vector2f winCenter = ((Vector2f)selectorType.getSize()) / 2.0f;
                        inputChoice.setOrigin(inputChoice.getLocalBounds().width / 2.0, inputChoice.getLocalBounds().height / 2.0);
                        inputChoice.setPosition(winCenter);

                        bool isMatrixInput = false;

                        while (selectorType.isOpen())
                        {
                            Event inputEvent;
                            while (selectorType.pollEvent(inputEvent))
                            {
                                if (inputEvent.type == Event::Closed)
                                {
                                    selectorType.close();
                                }
                                if (inputEvent.type == Event::KeyPressed)
                                {
                                    if (inputEvent.key.code == Keyboard::F)
                                    {
                                        isMatrixInput = true;
                                        selectorType.close();
                                    }
                                    else if (inputEvent.key.code == Keyboard::D)
                                    {
                                        selectorType.pollEvent(event);
                                        dims = querySelectorDEBUG(selectorType, font, "Enter Dimensions (RxC): ");
                                        numRows = stoi(dims.substr(0, dims.find('x')));
                                        numCols = stoi(dims.substr(dims.find('x') + 1));
                                        isManualInputClosed = true;
                                        selectorType.close();
                                    }
                                }
                            }
                            selectorType.clear(Color::White);
                            selectorType.draw(inputChoice);
                            selectorType.display();
                        }

                        if (isMatrixInput){
                        const unsigned int extraHeight = 50;
                        RenderWindow matrixSelector(VideoMode(600, 600 + extraHeight), "Select Dimensions [MATRIX INPUT]", Style::Close);
                        matrixSelector.setPosition(windowPosition);
                        unsigned int maxRows = 50;
                        unsigned int maxCols = 50;
                        float cellWidth = matrixSelector.getSize().x / static_cast<float>(maxCols);
                        float cellHeight = (matrixSelector.getSize().y - extraHeight) / static_cast<float>(maxRows);

                        int selectedRow = -1;
                        int selectedCol = -1;
                        int highlightRow = -1;
                        int highlightCol = -1;

                        liveDimensions.setPosition(10.f, matrixSelector.getSize().y - 20.f);
                        

                        while (matrixSelector.isOpen())
                        {
                            Event selectorEvent;
                            while (matrixSelector.pollEvent(selectorEvent))
                            {
                                switch (selectorEvent.type)
                                {
                                    case Event::Closed:
                                        matrixSelector.close();
                                        break;
                                    case Event::MouseMoved:
                                        // Update the highlighted cell when the mouse moves
                                        highlightRow = static_cast<int>(selectorEvent.mouseMove.y / cellHeight);
                                        highlightCol = static_cast<int>(selectorEvent.mouseMove.x / cellWidth);

                                        // Update the live dimensions text
                                        liveDimensions.setString("Selected Dimensions: " + to_string(highlightRow + 1) + "x" + to_string(highlightCol + 1));
                                        break;
                                    case Event::MouseButtonPressed:
                                        if (selectorEvent.mouseButton.button == Mouse::Left)
                                        {
                                            // Use the highlighted cell as the selected dimensions
                                            if (highlightRow >= 0 && highlightCol >= 0)
                                            {
                                                selectedRow = highlightRow;
                                                selectedCol = highlightCol;
                                                numRows = selectedRow + 1;
                                                numCols = selectedCol + 1;
                                            }
                                            matrixSelector.close();
                                            isMatrixSelectorClosed = true;
                                        }
                                        break;
                                }
                            }

                            matrixSelector.clear(Color(176, 224, 230));
                            for (unsigned int i = 0; i < maxRows; i++)
                            {
                                for (unsigned int j = 0; j < maxCols; j++)
                                {
                                    RectangleShape cell(Vector2f(cellWidth, cellHeight));
                                    cell.setPosition(j * cellWidth, i * cellHeight);
                                    cell.setOutlineColor(Color::Black);
                                    cell.setOutlineThickness(1.f);

                                    if (i <= highlightRow && j <= highlightCol)
                                    {
                                        RectangleShape highlightCell(Vector2f(cellWidth - 2.f, cellHeight - 2.f));
                                        highlightCell.setPosition(j * cellWidth + 1.f, i * cellHeight + 1.f);
                                        highlightCell.setFillColor(Color::Blue);
                                        highlightCell.setOutlineColor(Color::Black);
                                        highlightCell.setOutlineThickness(1.f);
                                        matrixSelector.draw(highlightCell);
                                    }
                                    else
                                    {
                                        cell.setFillColor(Color(176, 224, 230));
                                        matrixSelector.draw(cell);
                                    }
                                }
                            }

                            // Draw the live dimensions text in the matrix selector window
                            matrixSelector.draw(liveDimensions);

                            // Draw the cell outlines after drawing the blue highlight area
                            for (unsigned int i = 0; i < maxRows; i++)
                            {
                                for (unsigned int j = 0; j < maxCols; j++)
                                {
                                    RectangleShape cellOutline(Vector2f(cellWidth, cellHeight));
                                    cellOutline.setPosition(j * cellWidth, i * cellHeight);
                                    cellOutline.setFillColor(Color::Transparent);
                                    cellOutline.setOutlineColor(Color::Black);
                                    cellOutline.setOutlineThickness(1.f);
                                    matrixSelector.draw(cellOutline);
                                }
                            }

                            matrixSelector.display();
                        }
                    }
                    }

                    if (isMatrixSelectorClosed || isManualInputClosed)
                    {
                        p1->buildBoard(numRows, numCols);
                        p1->editBoard(window, font);
                        window.setView(window.getDefaultView());
                    }
                    else if (event.key.code == Keyboard::F)
                    {
                        window.pollEvent(event);
                        fileName = querySelectorDEBUG(window, font, "Enter File Name (No Extension): ");
                        p1->buildBoard(fileName);
                        p1->editBoard(window, font);
                        window.setView(window.getDefaultView());
                    }
                    break;
            }
        }
        window.draw(mainMenuSprite);
        window.display();
    }

    return 0;
}

// ... breadthFirstSearch and depthFirstSearch functions ...

string querySelectorDEBUG(RenderWindow& window, Font& font, string query)
{
    Vector2f winCenter = ((Vector2f)window.getSize()) / 2.0f;
    Text fileSelect(query, font, 24);
    fileSelect.setOrigin(fileSelect.getLocalBounds().width / 2.0, fileSelect.getLocalBounds().height / 2.0);
    fileSelect.setFillColor(Color::Black);
    fileSelect.setPosition(winCenter);
    string input;
    while (window.isOpen())
    {
        window.clear(Color::Cyan);
        Event event;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
                case Event::Closed:
                    window.close();
                    break;
                case Event::KeyPressed:
                    if (event.key.code == Keyboard::Enter)
                    {
                        return input;
                    }
                    else if ((event.key.code == Keyboard::Backspace) && (input.size() > 0))
                    {
                        input = input.substr(0, input.size() - 1);
                        fileSelect.setString(query + input);
                    }
                    break;
                case Event::TextEntered:
                    if (isalnum(static_cast<char>(event.text.unicode)))
                    {
                        input += static_cast<char>(event.text.unicode);
                        fileSelect.setString(query + input);
                    }
                    break;
            }
        }
        window.draw(fileSelect);
        window.display();
    }
    if (query == "Enter File Name (No Extension): ")
    {
        return "map";
    }
    else
    {
        return "10x10";
    }
}
