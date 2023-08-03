#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <map>
#include <string>
#include "Player.h"
#include "ButtonMaker.h"
#include "MusicPlayer.h"
#include "MatrixSelector.h"
#include "iostream"

using namespace std;
using namespace sf;

string querySelectorDEBUG(RenderWindow& window, Font& font, string query, string imagePath);

int main(int argc, char const *argv[])
{
    Tile::loadTextures();
    Font font;
    font.loadFromFile("files/font.ttf");
    //    unsigned int monitorWidth = VideoMode::getDesktopMode().width;
    //    unsigned int monitorHeight = VideoMode::getDesktopMode().height;
    unsigned int numRows = 10;
    unsigned int numCols = 10;
    string fileName = "map";
    string dims;

    // load menu textures
    sf::Texture mainMenuTexture;
    mainMenuTexture.loadFromFile("files/images/main_menu.png");

    sf::Texture dimensionMenuTexture;
    mainMenuTexture.loadFromFile("files/images/main_menu.png");

    sf::Texture loadMenuTexture;
    mainMenuTexture.loadFromFile("files/images/main_menu.png");

    RenderWindow window(VideoMode(mainMenuTexture.getSize().x, mainMenuTexture.getSize().y), "Trace Race", Style::Default);

        // Set the initial window position to the center of the screen
    //    sf::Vector2i screenCenter(monitorWidth / 2, monitorHeight / 2);
    //    sf::Vector2i windowPosition(screenCenter.x - window.getSize().x / 2, screenCenter.y - window.getSize().y / 2);
    //    window.setPosition(windowPosition);

    window.setFramerateLimit(360);

      // music player
    MusicPlayer backgroundMusic;




    // load background / adjust scaling
    float scaleX = (float)window.getSize().x / mainMenuTexture.getSize().x;
    float scaleY = (float)window.getSize().y / mainMenuTexture.getSize().y;
    sf::Sprite mainMenuSprite;
    mainMenuSprite.setTexture(mainMenuTexture);
    mainMenuSprite.setScale(scaleX, scaleY);

    ButtonMaker newGameButton(0.0f, 222.0f, 315.0f, 52.0f);
    ButtonMaker loadGameButton(0.0f, 319.0f, 315.0f, 52.0f);
    ButtonMaker exitGameButton(477.0f, 477.0f, 106.0f, 36.0f);

    Player* p1 = new Player("Debug");

    while (window.isOpen())
    {
        bool isMatrixSelectorClosed = false;
        bool isManualInputClosed = false;
        Vector2i  mousePosition = Mouse::getPosition(window);
        // DEBUG SECTION
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

                mainMenuSprite.setScale(scaleX, scaleY);


                // move clickables
                newGameButton.updateScale(scaleX, scaleY);
            }
            switch (event.type)
            {
                // close window
                case Event::Closed:
                    window.close();
                    break;
                // create dimension type selector menu

                // mute music
                case Event::KeyPressed:
                    if (event.key.code == Keyboard::M) {
                        backgroundMusic.toggleMute();
                    }
                    break;

                case Event::MouseButtonPressed:

                    if (newGameButton.isClicked(mousePosition))
                                    {
                        // input selection
                        //TODO: CREATE MENU FOR INPUT SELECTION
                        RenderWindow selectorType(VideoMode(600,600), "Choose Dimensional Input", Style::Close);
                        selectorType.setFramerateLimit(360);
                        Text inputChoice("f- Matrix Input\n" "d- Manual Input \n", font, 24);
                        inputChoice.setFillColor(Color::Black);
                        Vector2f winCenter = ((Vector2f)selectorType.getSize())/2.0f;
                        inputChoice.setOrigin(inputChoice.getLocalBounds().width/2.0, inputChoice.getLocalBounds().height/2.0);
                        inputChoice.setPosition(winCenter);

                        bool isMatrixInput = false;

                        while (selectorType.isOpen()) {
                            Event inputEvent;


                            while (selectorType.pollEvent(inputEvent)) {
                                if (inputEvent.type == Event::Closed) {
                                    selectorType.close();
                                }
                                // mute with m
                                if (inputEvent.type == Event::KeyPressed) {
                                    if (inputEvent.key.code == Keyboard::M) {
                                        backgroundMusic.toggleMute();
                                    }
                                }
                                // f = select matrix input
                                if (inputEvent.type == Event::KeyPressed) {
                                    if (inputEvent.key.code == Keyboard::F) {
                                        isMatrixInput = true;
                                        selectorType.close();
                                    }
                                        // d = manually enter dimensions
                                    else if (inputEvent.key.code == Keyboard::D) {
                                        selectorType.pollEvent(event);
                                        dims = querySelectorDEBUG(selectorType, font, "Enter Dimensions (RxC): ", "");
                                        numRows = stoi(dims.substr(0, dims.find('x')));
                                        numCols = stoi(dims.substr(dims.find('x')+1));
                                        isManualInputClosed = true;
                                        selectorType.close();
                                    }
                                }
                            }
                            selectorType.clear(Color::White);
                            selectorType.draw(inputChoice);
                            selectorType.display();
                        }
                        // create dimensional matrix selector
                        if (isMatrixInput) {
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
                                            // when size is selected, close menu
                                            if (selectorEvent.mouseButton.button == Mouse::Left) {
                                                numRows = static_cast<int>(selectorEvent.mouseButton.y / cellHeight) + 1;
                                                numCols = static_cast<int>(selectorEvent.mouseButton.x / cellWidth) + 1;
                                                matrixSelector.close();
                                                isMatrixSelectorClosed = true;
                                            }
                                            break;
                                    }
                                    //TODO: MAKE MUSIC MUTABLE EVERYWHERE
                                    switch (event.type){
                                        // mute with m
                                        case Event::KeyPressed:
                                            if (event.key.code == Keyboard::M) {
                                                backgroundMusic.toggleMute();
                                            }
                                            break;
                                    }
                                }
                                Vector2i mousePositionMatrixSelector = Mouse::getPosition(matrixSelector);
                                matrixSelector.clear(Color::White);
                                // create grid
                                for (unsigned int i = 0; i < maxRows; i++) {
                                    for (unsigned int j = 0; j < maxCols; j++) {
                                        RectangleShape cell(Vector2f(cellWidth, cellHeight));
                                        cell.setPosition(j * cellWidth, i * cellHeight);
                                        cell.setOutlineColor(Color::Black);
                                        cell.setOutlineThickness(1.f);

                                        if (i * cellHeight <= mousePositionMatrixSelector.y && j * cellWidth <= mousePositionMatrixSelector.x &&
                                            (i + 1) * cellHeight > mousePositionMatrixSelector.y && (j + 1) * cellWidth > mousePositionMatrixSelector.x) {
                                            // highlight cells over entire matrix as mouse rolls over
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
                    // run game (starting with Player 1)
                    if (isMatrixSelectorClosed or isManualInputClosed)
                    {
                        // Enter edit mode for player 1
                        p1->buildBoard(numRows, numCols);
                        p1->editBoard(window, font);
                        window.setView(window.getDefaultView());
                    }
                    // pull load game menu
                    if (loadGameButton.isClicked(mousePosition))
                    {
                        // Enter edit mode for player 1 based on file
                        window.pollEvent(event);
                        fileName = querySelectorDEBUG(window, font, "Enter File Name (No Extension): ", "");
                        p1->buildBoard(fileName);
                        p1->editBoard(window, font);
                        window.setView(window.getDefaultView());
                    }
                    // exit game
                    if (exitGameButton.isClicked(mousePosition)){
                        window.close();

                    }
                    break;
            }
        }
        window.draw(mainMenuSprite);
        window.display();
    }
    

    return 0;
}

string querySelectorDEBUG(RenderWindow& window, Font& font, string query, string imagePath)
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