#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include "Player.h"
#include "iostream"

using namespace std;
using namespace sf;

string querySelectorDEBUG(RenderWindow& window, Font& font, string query);

int main(int argc, char const *argv[])
{
    Tile::loadTextures();
    Font font;
    font.loadFromFile("files/font.ttf");
    unsigned int numRows = 10;
    unsigned int numCols = 10;
    string fileName = "map";
    string dims;

    sf::Texture mainMenuTexture;
    mainMenuTexture.loadFromFile("files/images/main_menu.png");

    RenderWindow window(VideoMode(mainMenuTexture.getSize().x, mainMenuTexture.getSize().y), "Trace Race", Style::Default);
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

    while (window.isOpen())
    {

        // DEBUG SECTION
        Vector2i  mousePosition = Mouse::getPosition(window);
        bool isMatrixSelectorOpen = false;
        // MOUSE DEBUGGER
//        cout << "x: "<< mousePosition.x << " y: "<< mousePosition.y << endl;

        window.clear(Color::Blue);

        Event event;
        while (window.pollEvent(event))
        {
            // make window resizable
            if (event.type == sf::Event::Resized) {
                sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                window.setView(sf::View(visibleArea));

                scaleX = (float)event.size.width / mainMenuTexture.getSize().x;
                scaleY = (float)event.size.height / mainMenuTexture.getSize().y;

                mainMenuSprite.setScale(scaleX, scaleY);


                // move clickables
                newGameZone.left = newGameX * scaleX;
                newGameZone.top = newGameY * scaleY;
                newGameZone.width = newGameWidth * scaleX;
                newGameZone.height = newGameHeight * scaleY;
            }
            switch (event.type)
            {
                // close window
                case Event::Closed:
                    window.close();
                    break;
                // create dimension type selector menu
                case Event::MouseButtonPressed:
                    if (newGameZone.contains(event.mouseButton.x, event.mouseButton.y))
                                    {
                        // input selection
                        RenderWindow selectorType(VideoMode(600,600), "Choose Dimensional Input", Style::Close);
                        selectorType.setFramerateLimit(360);
                        Text inputChoice("f- Matrix Input\n" "d- Manual Input \n", font, 24);
                        inputChoice.setFillColor(Color::Black);
                        Vector2f winCenter = ((Vector2f)selectorType.getSize())/2.0f;
                        inputChoice.setOrigin(inputChoice.getLocalBounds().width/2.0, inputChoice.getLocalBounds().height/2.0);
                        inputChoice.setPosition(winCenter);

                        bool matrixInput = false;

                        while (selectorType.isOpen()) {
                            Event inputEvent;
                            while (selectorType.pollEvent(inputEvent)) {
                                if (inputEvent.type == Event::Closed) {
                                    selectorType.close();
                                }
                                // f = select matrix input
                                if (inputEvent.type == Event::KeyPressed) {
                                    if (inputEvent.key.code == Keyboard::F) {
                                        matrixInput = true;
                                        selectorType.close();
                                    }
                                        // d = manually enter dimensions
                                    else if (inputEvent.key.code == Keyboard::D) {
                                        selectorType.pollEvent(event);
                                        dims = querySelectorDEBUG(selectorType, font, "Enter Dimensions (RxC): ");
                                        numRows = stoi(dims.substr(0, dims.find('x')));
                                        numCols = stoi(dims.substr(dims.find('x')+1));
                                        selectorType.close();
                                    }
                                }
                            }
                            selectorType.clear(Color::White);
                            selectorType.draw(inputChoice);
                            selectorType.display();
                        }
                        // create dimensional matrix selector
                        if (matrixInput) {
                            // Matrix Selector Section
                            RenderWindow matrixSelector(VideoMode(600, 600), "Select Race Dimensions [MATRIX INPUT]", Style::Close);
                            unsigned int maxRows = 50;
                            unsigned int maxCols = 50;
                            float cellWidth = matrixSelector.getSize().x / static_cast<float>(maxCols);
                            float cellHeight = matrixSelector.getSize().y / static_cast<float>(maxRows);

                            while (matrixSelector.isOpen()) {
                                Event selectorEvent;
                                while (matrixSelector.pollEvent(selectorEvent)) {
                                    switch (selectorEvent.type) {
                                        case Event::Closed:
                                            matrixSelector.close();
                                            break;
                                        case Event::MouseButtonPressed:
                                            if (selectorEvent.mouseButton.button == Mouse::Left) {
                                                numRows = static_cast<int>(selectorEvent.mouseButton.y / cellHeight) + 1;
                                                numCols = static_cast<int>(selectorEvent.mouseButton.x / cellWidth) + 1;
                                                matrixSelector.close();
                                                isMatrixSelectorOpen = true;
                                                cout << "matrix closed" << endl;
                                            }
                                            break;
                                    }
                                }
                                Vector2i mousePosition = Mouse::getPosition(matrixSelector);
                                matrixSelector.clear(Color::White);
                                for (unsigned int i = 0; i < maxRows; i++) {
                                    for (unsigned int j = 0; j < maxCols; j++) {
                                        RectangleShape cell(Vector2f(cellWidth, cellHeight));
                                        cell.setPosition(j * cellWidth, i * cellHeight);
                                        cell.setOutlineColor(Color::Black);
                                        cell.setOutlineThickness(1.f);

                                        if (i * cellHeight <= mousePosition.y && j * cellWidth <= mousePosition.x &&
                                            (i + 1) * cellHeight > mousePosition.y && (j + 1) * cellWidth > mousePosition.x) {
                                            // highlight cells over entire matrix
                                            for (int r = 0; r <= i; r++) {
                                                for (int c = 0; c <= j; c++) {
                                                    RectangleShape highlightCell(Vector2f(cellWidth, cellHeight));
                                                    highlightCell.setPosition(c * cellWidth, r * cellHeight);
                                                    highlightCell.setFillColor(Color::Blue);
                                                    highlightCell.setOutlineColor(Color::Black);
                                                    highlightCell.setOutlineThickness(1.f);
                                                    matrixSelector.draw(highlightCell);

                                                }
                                            }
                                            // try to fix this later, display matrix size as player adjusts
                                            string sizeText = to_string(i + 1) + "x" + to_string(j + 1);
                                            // size
                                            Text dimensionsText(sizeText, font, 24);
                                            // color
                                            dimensionsText.setFillColor(Color::Black);
                                            // position
                                            dimensionsText.setPosition(j * cellWidth, i * cellHeight);
                                            matrixSelector.draw(dimensionsText);
                                        }
                                        else {
                                            cell.setFillColor(Color::White);
                                        }
                                        matrixSelector.draw(cell);
                                    }
                                }
                                matrixSelector.display();
                            }
                            // end matrix selector
                        }
                    }
                    if (isMatrixSelectorOpen)
                    {

                        // Enter edit mode for player 1
                        p1->buildBoard(numRows, numCols);
                        p1->editBoard(window, font);
                        window.setView(window.getDefaultView());
                    }
                    else if (event.key.code == Keyboard::F)
                    {
                        // Enter edit mode for player 1 based on file
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

string querySelectorDEBUG(RenderWindow& window, Font& font, string query)
{
    // In the final version this would be split into two routines
    // Dimension selector would have a gui showing an interactive matrix (similar to the table insert function in ms word)
    // Filename selector would be a drop down menu showing all of the files from files/

    Vector2f winCenter = ((Vector2f)window.getSize())/2.0f;
    Text fileSelect(query, font, 24);
    fileSelect.setOrigin(fileSelect.getLocalBounds().width/2.0, fileSelect.getLocalBounds().height/2.0);
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
                    input = input.substr(0, input.size()-1);
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