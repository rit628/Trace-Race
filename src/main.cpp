#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <map>
#include <string>
#include <filesystem>
#include "Player.h"
#include "ButtonMaker.h"
#include "MusicPlayer.h"
#include "MatrixSelector.h"
#include "iostream"

using namespace std;
using namespace sf;

string loadMenu(RenderWindow& window, Font& font, const string& query, const sf::Sprite& backgroundSprite);
void dimensionMenu(RenderWindow& window, const sf::Sprite& backgroundSprite, sf::Sprite& manualMenuSprite, bool& isMatrixClosed, bool& isManualClosed, unsigned int& numRows, unsigned int& numCols, Font& font);
string manualMenu(RenderWindow& window, Font& font, const string& query, const sf::Sprite& backgroundSprite);

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
    dimensionMenuTexture.loadFromFile("files/images/dimension_menu.png");

    sf::Texture loadMenuTexture;
    loadMenuTexture.loadFromFile("files/images/load_menu.png");

    sf::Texture manualMenuTexture;
    manualMenuTexture.loadFromFile("files/images/manual_menu.png");

    // create sprites
    sf::Sprite mainMenuSprite;
    sf::Sprite dimensionMenuSprite;
    sf::Sprite loadMenuSprite;
    sf::Sprite manualMenuSprite;

    mainMenuSprite.setTexture(mainMenuTexture);
    dimensionMenuSprite.setTexture(dimensionMenuTexture);
    loadMenuSprite.setTexture(loadMenuTexture);
    manualMenuSprite.setTexture(manualMenuTexture);

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
                    // start new game
                    if (newGameButton.isClicked(mousePosition)){
                        bool isMatrixInput = false;
                        dimensionMenu(window, dimensionMenuSprite, manualMenuSprite, isMatrixInput, isManualInputClosed, numRows, numCols, font);
                        window.setFramerateLimit(360);

                        // create dimensional matrix selector
                        if (isMatrixInput) {
                            // Matrix Selector Section
                            RenderWindow matrixSelector(VideoMode(600, 600), "Select Race Dimensions [MATRIX INPUT]", Style::Close);
                            MatrixSelector matrixSelector1(50, 50);
                            matrixSelector1.drawGrid(matrixSelector, numRows, numCols, isMatrixSelectorClosed);
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
                    // load game menu
                    if (loadGameButton.isClicked(mousePosition))
                    {
                        // Enter edit mode for player 1 based on file
                        window.pollEvent(event);
                        fileName = loadMenu(window, font, "Enter File Name (No Extension): \n          ",
                                            loadMenuSprite);
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

string loadMenu(RenderWindow& window, Font& font, const string& query, const sf::Sprite& backgroundSprite)
{
    // Filename selector would be a drop down menu showing all of the files from files/

    Vector2f winCenter = ((Vector2f)window.getSize())/2.0f;
    Text fileSelect(query, font, 24);
    fileSelect.setOrigin(fileSelect.getLocalBounds().width/2.0, fileSelect.getLocalBounds().height/2.0);
    fileSelect.setFillColor(Color::White);
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
        window.draw(backgroundSprite);
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

void dimensionMenu(RenderWindow& window, const sf::Sprite& backgroundSprite, sf::Sprite& manualMenuSprite, bool& isMatrixClosed, bool& isManualClosed, unsigned int& numRows, unsigned int& numCols, Font& font)
{
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
                    if (event.key.code == Keyboard::F)
                    {
                        isMatrixClosed = true;
                        return;
                    }
                    else if (event.key.code == Keyboard::D)
                    {
                        window.pollEvent(event);
                        string dims = manualMenu(window, font, "Enter Dimensions (RxC): \n           ",
                                                        manualMenuSprite);
                        numRows = stoi(dims.substr(0, dims.find('x')));
                        numCols = stoi(dims.substr(dims.find('x')+1));
                        isManualClosed = true;
                        return;
                    }
                    break;
            }
        }
        window.draw(backgroundSprite);
        window.display();
    }
}

string manualMenu(RenderWindow& window, Font& font, const string& query, const sf::Sprite& backgroundSprite)
{
    Vector2f winCenter = ((Vector2f)window.getSize())/2.0f;
    Text fileSelect(query, font, 24);
    fileSelect.setOrigin(fileSelect.getLocalBounds().width/2.0, fileSelect.getLocalBounds().height/2.0);
    fileSelect.setFillColor(Color::White);
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
        window.draw(backgroundSprite);
        window.draw(fileSelect);
        window.display();
    }
    return "";
}