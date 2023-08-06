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
#include <thread>
#include <atomic>
#include <chrono>
#include <sstream>
#include <iomanip>

using namespace std;
using namespace sf;

// initialize menus and final race menu
string loadMenu(RenderWindow& window, Font& font, const string& query, const sf::Sprite& backgroundSprite);
void dimensionMenu(RenderWindow& window, const sf::Sprite& backgroundSprite, sf::Sprite& manualMenuSprite, bool& isMatrixClosed, bool& isManualClosed, unsigned int& numRows, unsigned int& numCols, Font& font);
string manualMenu(RenderWindow& window, Font& font, const string& query, const sf::Sprite& backgroundSprite);
void battle(RenderWindow& window, Player* p1, Player* p2, Font& font);
Player& race(Board& b, Player& p1, Player& p2);

int main(int argc, char const *argv[])
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

    // render main window
    RenderWindow window(VideoMode(mainMenuTexture.getSize().x, mainMenuTexture.getSize().y), "Trace Race", Style::Default);

    // Set the initial window position to the center of the screen
    sf::Vector2i screenCenter(monitorWidth / 2, monitorHeight / 2);
    sf::Vector2i windowPosition(screenCenter.x - window.getSize().x / 2, screenCenter.y - window.getSize().y / 2);
    window.setPosition(windowPosition);
    window.setFramerateLimit(360);

    // music player
    MusicPlayer backgroundMusic;

    // load background / adjust scaling
    float scaleX = (float)window.getSize().x / mainMenuTexture.getSize().x;
    float scaleY = (float)window.getSize().y / mainMenuTexture.getSize().y;
    mainMenuSprite.setTexture(mainMenuTexture);
    mainMenuSprite.setScale(scaleX, scaleY);

    // create buttons for main menu
    ButtonMaker newGameButton(0.0f, 222.0f, 315.0f, 52.0f);
    ButtonMaker loadGameButton(0.0f, 319.0f, 315.0f, 52.0f);
    ButtonMaker exitGameButton(477.0f, 477.0f, 106.0f, 36.0f);

    // create player objects
    Player* p1 = new Player("Player 1", 1);
    Player* p2 = new Player("Bot", 2);

    while (window.isOpen())
    {
        // create bool(s) for menu switching
        bool isMatrixSelectorClosed = false;
        bool isManualInputClosed = false;

        // track mouse for clicks
        Vector2i  mousePosition = Mouse::getPosition(window);

        // refresh menu
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
                        string response = loadMenu(window, font, "AI Opponent? (Y\\N):\n        ", loadMenuSprite);
                        if (response == "N" || response == "n")
                        {
                            p2->setName("Player 2");
                        }
                        else
                        {
                            p2->setName("Bot");
                        } 
                        bool isMatrixInput = false;

                        // create dimension menu
                        dimensionMenu(window, dimensionMenuSprite, manualMenuSprite, isMatrixInput, isManualInputClosed, numRows, numCols, font);
                        window.setFramerateLimit(360);

                        // create dimensional matrix selector
                        if (isMatrixInput) {
                            // Matrix Selector Section
                            MatrixSelector matrixSelector1(50, 50);
                            matrixSelector1.drawGrid(window, numRows, numCols, isMatrixSelectorClosed);
                        }
                    }
                    // run game (starting with Player 1)
                    if (isMatrixSelectorClosed or isManualInputClosed)
                    {
                        // Enter edit mode for player 1
                        p1->buildBoard(numRows, numCols);
                        unsigned int finishCol = p1->editBoard(window, font);
                        p2->buildBoard(numRows, numCols);
                        p2->editBoard(window, font, finishCol);
                        window.setView(window.getDefaultView());
                        battle(window, p1, p2, font);                   
                    }
                    // load game menu
                    if (loadGameButton.isClicked(mousePosition))
                    {
                        // Enter edit mode for player 1 based on file
                        window.pollEvent(event);
                        fileName = loadMenu(window, font, "Enter File Name (No Extension): \n             ",
                                            loadMenuSprite);
                        window.pollEvent(event);
                        string response = loadMenu(window, font, "AI Opponent? (Y\\N):\n        ", loadMenuSprite);
                        if (response == "N" || response == "n")
                        {
                            p2->setName("Player 2");
                        }
                        else
                        {
                            p2->setName("Bot");
                        }
                        window.pollEvent(event);
                        pair<int, int> dims = p1->buildBoard(fileName);
                        unsigned int finishCol = p1->editBoard(window, font);
                        p2->buildBoard(dims.first, dims.second);
                        p2->editBoard(window, font, finishCol);
                        battle(window, p1, p2, font);
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
    
    delete p1;
    delete p2;
    return 0;
}

// load menu function
string loadMenu(RenderWindow& window, Font& font, const string& query, const sf::Sprite& backgroundSprite)
{
    // set text in center of screen for file loading
    Vector2f winCenter = ((Vector2f)window.getSize())/2.0f;
    Text fileSelect(query, font, 24);
    fileSelect.setOrigin(fileSelect.getLocalBounds().width/2.0, fileSelect.getLocalBounds().height/2.0);
    fileSelect.setFillColor(Color::White);
    fileSelect.setPosition(winCenter);
    string input;

    // while window is open allow player to load game
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
                    window.pollEvent(event);
                    return input;
                }
                // Remove text from render window
                else if ((event.key.code == Keyboard::Backspace) && (input.size() > 0))
                {
                    input = input.substr(0, input.size()-1);
                    fileSelect.setString(query + input);
                }
                break;
            // Add text to render window
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
// create dimension menu for matrix selection or push player to manual menu
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
                // f = choose matrix input
                case Event::KeyPressed:
                    if (event.key.code == Keyboard::F)
                    {
                        isMatrixClosed = true;
                        return;
                    }
                    // d = manually enter dimensions
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
// manually enter dimensions menu
string manualMenu(RenderWindow& window, Font& font, const string& query, const sf::Sprite& backgroundSprite)
{
    // center text
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
                    // Remove text from render window
                    else if ((event.key.code == Keyboard::Backspace) && (input.size() > 0))
                    {
                        input = input.substr(0, input.size()-1);
                        fileSelect.setString(query + input);
                    }
                    break;
                // Add text to render window
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

// final menu "battle" function
void battle(RenderWindow& window, Player* p1, Player* p2, Font& font)
{
    
    Board final = p1->combineBoard(*p2);
    bool panning = false;
    Vector2f m0, m1;
    Vector2i pixel;
    sf::View camera(sf::FloatRect(0, 0, 250, 250));
    View dflt = window.getDefaultView();
    window.setSize(Vector2u(window.getSize().x*1.5, window.getSize().y*1.5));
    dflt.setSize(static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y));
    window.setView(dflt);

    // Update the initial position and zoom level of the camera view
    float mapWidth = static_cast<float>(final.numCols * final.tileDim);
    float mapHeight = static_cast<float>(final.numRows * final.tileDim);
    float aspectRatio = static_cast<float>(window.getSize().x) / static_cast<float>(window.getSize().y);
    float viewWidth = aspectRatio * mapHeight;
    camera.setSize(viewWidth, mapHeight);
    camera.setViewport(sf::FloatRect(.4, .15, 1, 1));

    // Adjust the position to move the map downwards and to the right
    float xOffset = -350.0f; // Adjust this value to move the map horizontally (right).
    float yOffset = -670.0f; // Adjust this value to move the map vertically (down).
    camera.setCenter(mapWidth, mapHeight/3);
    camera.zoom(0.7f);

    // Create text objects for BFS, DFS, and Overlapping algorithm descriptions
    sf::Text bfsText(p1->getDisplayStr(), font, 24);
    bfsText.setFillColor(sf::Color(63, 72, 204));
    bfsText.setStyle(sf::Text::Bold);
    bfsText.setPosition(10, 180);

    sf::Text dfsText(p2->getDisplayStr(), font, 24);
    dfsText.setFillColor(sf::Color(255, 127, 39));
    dfsText.setStyle(sf::Text::Bold);
    dfsText.setPosition(10, 240);

    sf::Text overlappingText("Overlap", font, 24);
    overlappingText.setFillColor(sf::Color(0, 128, 0)); // Dark green (R=0, G=128, B=0)
    overlappingText.setStyle(sf::Text::Bold);
    overlappingText.setPosition(10, 290);

    sf::Text raceText("Press 'R' to Race!\n\n\n\nKEY:", font, 28);
    raceText.setFillColor(sf::Color::Black);
    raceText.setStyle(sf::Text::Bold);
    raceText.setPosition(10, 20);


    // Create a view for the text area on the right side
    sf::View textView;
    textView.setSize(window.getSize().x - viewWidth, window.getSize().y);
    textView.setViewport(sf::FloatRect(viewWidth / static_cast<float>(window.getSize().x), 0.0f, 1.0f, 1.0f));
    window.setView(textView);
    
    while (window.isOpen())
    {
        Event event;
        window.clear(Color(230, 230, 250));
        
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
                case Event::Closed:
                    window.close();
                    break;
                case Event::MouseButtonPressed:
                    if (event.mouseButton.button == Mouse::Right)
                    {
                        panning = true;
                        m0 = window.mapPixelToCoords(Mouse::getPosition(window));
                    }
                    break;
                case Event::MouseButtonReleased:
                    if (event.mouseButton.button == Mouse::Right)
                    {
                        panning = false;
                    }
                    break;
                case Event::MouseMoved:
                    // If holding right click, pan the screen
                    if (panning)
                    {
                        m1 = window.mapPixelToCoords(Mouse::getPosition(window));
                        camera.setCenter(camera.getCenter() + m0 - m1);
                        camera.move(m0 - m1);
                        window.setView(camera);
                        m0 = window.mapPixelToCoords(Mouse::getPosition(window));
                    }
                    break;
                case Event::MouseWheelMoved:
                    // If scrolling wheel and not panning the screen, zoom in
                    if (!panning)
                    {
                        float zoom = (event.mouseWheel.delta >= 0) ? .5 : 2;
                        pixel = Vector2i(event.mouseButton.x, event.mouseButton.y);
                        m0 = window.mapPixelToCoords(pixel);
                        camera.zoom(zoom);
                        window.setView(camera);
                        m1 = window.mapPixelToCoords(pixel);
                        camera.move(m0 - m1);
                        window.setView(camera);
                    }
                    break;
                case Event::KeyPressed:
                    if ((event.key.code == Keyboard::Escape) || (event.key.code == Keyboard::Enter))
                    {
                        View dflt = window.getDefaultView();
                        window.setSize(Vector2u(window.getSize().x/1.5, window.getSize().y/1.5));
                        dflt.setSize(static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y));
                        window.setView(dflt);
                        return;
                    }
                    else if (event.key.code == Keyboard::R)
                    {
                        Player& winner = race(final, *p1, *p2);
                        stringstream winText;
                        winText << winner.getDisplayStr() << " wins!\nTime: " << std::fixed << std::setprecision(3) << winner.endTime << " ms" << endl;
                        raceText.setString(winText.str());
                    }
                    break;
                    
                    
            }
        }
        // Draw the text in the right side view
        
        window.setView(window.getDefaultView());
        window.draw(bfsText);
        window.draw(dfsText);
        window.draw(overlappingText);
        window.draw(raceText);

        // Draw the map in the camera view
        window.setView(camera);
        window.draw(final);

        window.display();

        
    }
}
// final race function
Player& race(Board& b, Player& p1, Player& p2)
{
    // atomic bool to terminate other thread once one algorithm finishes
    std::atomic<bool> raceFlag{ false };
    chrono::time_point<chrono::high_resolution_clock> start = chrono::high_resolution_clock::now();
    chrono::time_point<chrono::high_resolution_clock> end1;
    chrono::time_point<chrono::high_resolution_clock> end2;
    // Splits player 1 and 2 algorithm runs into different threads to allow for simultaneous execution (data race)
    std::thread p1T(p1.selectedAlgorithm, std::ref(b), b.start, b.finish, 1, std::ref(raceFlag), std::ref(end1));
    std::thread p2T(p2.selectedAlgorithm, std::ref(b), b.finish, b.start, 2, std::ref(raceFlag), std::ref(end2));

    p1T.join();
    p2T.join();
    
    // Compute end time
    chrono::duration<double, std::milli> dt1 = end1 - start;
    chrono::duration<double, std::milli> dt2 = end2 - start;
    // Return winner for rendering to screen
    if (dt1.count() < dt2.count())
    {
        p1.score++;
        p1.endTime = dt1.count();
        return p1;
    }
    else
    {
        p2.score++;
        p2.endTime = dt2.count();
        return p2;
    }    
}