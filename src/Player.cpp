#include "Player.h"
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <string>
#include <SFML/Window/Keyboard.hpp>
#include <iostream>

using namespace std;
using namespace sf;


//checking if push works 2

//hi just checking if this works

// Function to calculate the center position of a window relative to another window
Vector2i getCenteredPosition(const Window& parent, const Vector2u& size) {
    const Vector2i parentCenter(parent.getPosition().x + parent.getSize().x / 2, parent.getPosition().y + parent.getSize().y / 2);
    return Vector2i(parentCenter.x - size.x / 2 + 50, parentCenter.y - size.y / 2);
}

Player::Player(string name, int turn) : name(name), turn(turn) {}

Player::~Player()
{
    this->board.reset();
}

void Player::onClick(Vector2i pos)
{
    if ((pos.x < this->board.numCols) && (pos.y < this->board.numRows))
    {
        char state = 'N';
        if (this->turn == 1)
        {
            // If the cursor is over the last row and the first player is editing, add the finish point
            if (pos.y == this->board.numRows-1)
            {
                state = 'F';
            }
            // If the cursor is on any edge (excluding corners and the end row) add the start point
            else if (((pos.y == 0) && ((pos.x != 0) && (pos.x != this->board.numCols-1))) || ((pos.y != 0) && ((pos.x == 0) || (pos.x == this->board.numCols-1))))
            {
                state = 'S';
            }            
        }
        else
        {
            // If the second player is editing, they cannot change the end point
            if (pos.y == 0)
            {
                return;
            }
            // If the cursor is on any edge (excluding corners and the end row) add the start point
            else if (((pos.y == this->board.numRows-1) && ((pos.x != 0) && (pos.x != this->board.numCols-1))) || ((pos.y != this->board.numRows-1) && ((pos.x == 0) || (pos.x == this->board.numCols-1))))
            {
                state = 'S';
            }            
        }
        
        
        this->board.updateTile(pos.y, pos.x, state);
    }   
}

void Player::buildBoard(unsigned int numRows, unsigned int numCols)
{
    this->board.build(numRows, numCols);
}

void Player::buildBoard(string fileName)
{
    this->board.build(fileName);
}

Board Player::combineBoard(Player& p2)
{
    return this->board.combine(p2.board);
}

void Player::resetBoard(bool wall)
{
    this->board.resetTiles(wall);
}


unsigned int Player::generateBoard(unsigned int numRows, unsigned int numCols, int finishCol)
{
    // Add random algo choice
    this->board.generate(numRows, numCols, finishCol);
    return this->board.finish->id;
}

unsigned int Player::editBoard(RenderWindow& window, Font& font)
{
//    sf::Vector2i screenCenter(sf::VideoMode::getDesktopMode().width / 2, sf::VideoMode::getDesktopMode().height / 2);
//    sf::Vector2i windowPosition(screenCenter.x - window.getSize().x / 2, screenCenter.y - window.getSize().y / 2);
//    window.setPosition(windowPosition);
    bool panning = false;
    bool editing = false;
    int selectedOption = -1; // Default to no option selected
    bool isAlgorithmSelectionWindowOpen = false;

    Vector2i pixel;
    Vector2i tileHover;
    vector<Vector2i> tileStack;
    sf::View camera(sf::FloatRect(0, 0, 250, 250));
    camera.setCenter(static_cast<float>(this->board.numCols * 16), static_cast<float>(this->board.numRows * 16));
    float topLimit = 0.22;
    float bottomLimit = 0.90;
    camera.setViewport(sf::FloatRect(0, topLimit, 1, bottomLimit - topLimit));
    window.setView(camera);
    Vector2f m0;
    Vector2f m1;
    Vector2f coords;
    string fileName;
    const int buttonWidth = 120;
    const int buttonHeight = 30;
    float deltaTime;

    sf::RectangleShape clearGridButton(sf::Vector2f(buttonWidth, buttonHeight));
    clearGridButton.setFillColor(sf::Color(66, 135, 245));
    clearGridButton.setPosition(10, 10);

    sf::Text clearGridText("Clear Grid", font, 18);
    clearGridText.setFillColor(sf::Color::White);
    clearGridText.setPosition(clearGridButton.getPosition().x + (buttonWidth - clearGridText.getLocalBounds().width) / 2,
        clearGridButton.getPosition().y + (buttonHeight - clearGridText.getLocalBounds().height) / 2);

    sf::RectangleShape chooseAlgorithmButton(sf::Vector2f(buttonWidth + 60, buttonHeight));
    chooseAlgorithmButton.setFillColor(sf::Color(66, 135, 245));
    chooseAlgorithmButton.setPosition(window.getSize().x - buttonWidth - 80, clearGridButton.getPosition().y);

    sf::Text chooseAlgorithmText("Choose Algorithm", font, 18);
    chooseAlgorithmText.setFillColor(sf::Color::White);
    chooseAlgorithmText.setPosition(chooseAlgorithmButton.getPosition().x + 30 + (buttonWidth - (chooseAlgorithmText.getLocalBounds().width)) / 2,
        chooseAlgorithmButton.getPosition().y + (buttonHeight - chooseAlgorithmText.getLocalBounds().height) / 2);

    float timer = 30.0f;
    bool isTimerRunning = true;
    sf::Clock clock;
    const int timerBoxWidth = 240;
    const int timerBoxHeight = 30;

    sf::RectangleShape timerBox(sf::Vector2f(timerBoxWidth, timerBoxHeight));
    timerBox.setFillColor(sf::Color(66, 135, 245));
    timerBox.setPosition((window.getSize().x - timerBoxWidth) / 2, window.getSize().y - timerBoxHeight - 10);

    sf::Text timerText("Time left: " + std::to_string(static_cast<int>(timer)) + " seconds", font, 18);
    timerText.setFillColor(sf::Color::White);
    timerText.setPosition(timerBox.getPosition().x + (timerBoxWidth - timerText.getLocalBounds().width) / 2,
        timerBox.getPosition().y + (timerBoxHeight - timerText.getLocalBounds().height) / 2);

    // Add this line before creating the sf::Text objects in the code
    const int editorTextSize = 14; // Choose the desired size

    // Replace the existing "editorText" definition with the following
    sf::Text editorText("Editor Keybindings:\n\nLeft Click (M1)- Edit Grid\nRight Click (M2)- Pan Grid\n"
        "Scroll Wheel Up/Down- Zoom In/Out\nEnter- Save Map to File\nEscape- Return to Menu\nG- Procedurally Generate Board\nC- Remove Disconnected Paths", font, editorTextSize);
    editorText.setOrigin(editorText.getLocalBounds().width / 2.0, editorText.getLocalBounds().height / 2.0);
    editorText.setFillColor(sf::Color::White);
    editorText.setPosition(editorText.getLocalBounds().width + 10, editorText.getLocalBounds().height - 60);

    // Make the editorText bold
    editorText.setStyle(sf::Text::Bold);

    const float minCameraY = window.getSize().y - timerBoxHeight - 20;

    while (window.isOpen())
    {
        window.clear(sf::Color(173, 216, 230));
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
                        window.setView(window.getDefaultView());
                        fileName = getFileName(window, font);
                        this->board.writeToFile(fileName);
                        this->board.clean();
                        if (!this->board.isValid())
                        {
                            this->board.generate(this->board.numRows, this->board.numCols);
                        }
                        return this->board.finish->id % this->board.numCols;
                    }
                    else if (event.key.code == Keyboard::G)
                    {
                        this->board.generate(this->board.numRows, this->board.numCols);
                    }
                    else if (event.key.code == Keyboard::C)
                    {
                        this->board.clean();
                    }
                    
                    else if (event.key.code == Keyboard::Escape)
                    {
                        this->board.clean();
                        if (!this->board.isValid())
                        {
                            this->board.generate(this->board.numRows, this->board.numCols);
                        }
                        return this->board.finish->id % this->board.numCols;
                    }
                    break;

            case Event::MouseButtonPressed:
                if (event.mouseButton.button == Mouse::Left)
                {
                    editing = true;
                    coords = window.mapPixelToCoords(Vector2i(event.mouseButton.x, event.mouseButton.y));
                    tileHover.x = floor(coords.x / board.tileDim);
                    tileHover.y = floor(coords.y / board.tileDim);
                    tileStack.push_back(tileHover);
                    onClick(tileHover);
                    window.setView(window.getDefaultView());
                    coords = window.mapPixelToCoords(Vector2i(event.mouseButton.x, event.mouseButton.y));
                    sf::Vector2f buttonPosition = clearGridButton.getPosition();
                    sf::FloatRect clearGridArea(clearGridText.getGlobalBounds());
                    sf::FloatRect chooseAlgorithmArea(chooseAlgorithmButton.getPosition(), sf::Vector2f(buttonWidth + 60, buttonHeight));
                    if (clearGridArea.contains(coords))
                    {
                        this->resetBoard();
                    }
                    else if (chooseAlgorithmArea.contains(coords))
                    {
                        // Show the algorithm selection window
                        isAlgorithmSelectionWindowOpen = true;
                        RenderWindow algorithmSelectionWindow(VideoMode(200, 100), "Choose Algorithm", Style::Titlebar | Style::Close);
                        algorithmSelectionWindow.setPosition(getCenteredPosition(window, Vector2u(200, 100)));
                        algorithmSelectionWindow.setFramerateLimit(60);

                        while (algorithmSelectionWindow.isOpen())
                        {
                            Event algorithmEvent;
                            while (algorithmSelectionWindow.pollEvent(algorithmEvent))
                            {
                                switch (algorithmEvent.type)
                                {
                                case Event::Closed:
                                    algorithmSelectionWindow.close();
                                    break;

                                case Event::MouseButtonPressed:
                                    if (algorithmEvent.mouseButton.button == Mouse::Left)
                                    {
                                        // Check if Option 1 is clicked
                                        if (algorithmEvent.mouseButton.x >= 0 && algorithmEvent.mouseButton.x <= 200 &&
                                            algorithmEvent.mouseButton.y >= 0 && algorithmEvent.mouseButton.y <= 50)
                                        {
                                            cout << "Breadth-first search chosen" << endl;
                                            selectedAlgorithm = PathfindingAlgorithm::BreadthFirstSearch;

                                            algorithmSelectionWindow.close();
                                        }
                                        // Check if Option 2 is clicked
                                        else if (algorithmEvent.mouseButton.x >= 0 && algorithmEvent.mouseButton.x <= 200 &&
                                            algorithmEvent.mouseButton.y >= 50 && algorithmEvent.mouseButton.y <= 100)
                                        {
                                            cout << "Depth-first search chosen" << endl;
                                            selectedAlgorithm = PathfindingAlgorithm::DepthFirstSearch;

                                            algorithmSelectionWindow.close();
                                        }
                                    }
                                    break;
                                }

                                algorithmSelectionWindow.clear(Color(173, 216, 230));

                                // Draw Option 1
                                RectangleShape option1Rect(Vector2f(200, 50));
                                option1Rect.setFillColor(Color(66, 135, 245));
                                algorithmSelectionWindow.draw(option1Rect);

                                Text option1Text("Breadth-first", font, 18);
                                option1Text.setFillColor(Color::White);
                                option1Text.setOrigin(option1Text.getLocalBounds().width / 2.0f, option1Text.getLocalBounds().height / 2.0f);
                                option1Text.setPosition(100, 25);
                                algorithmSelectionWindow.draw(option1Text);

                                // Draw Option 2
                                RectangleShape option2Rect(Vector2f(200, 50));
                                option2Rect.setFillColor(Color(66, 135, 245));
                                option2Rect.setPosition(0, 50);
                                algorithmSelectionWindow.draw(option2Rect);

                                Text option2Text("Depth-first", font, 18);
                                option2Text.setFillColor(Color::White);
                                option2Text.setOrigin(option2Text.getLocalBounds().width / 2.0f, option2Text.getLocalBounds().height / 2.0f);
                                option2Text.setPosition(100, 75);
                                algorithmSelectionWindow.draw(option2Text);

                                algorithmSelectionWindow.display();
                            }
                        }
                        if (selectedAlgorithm == PathfindingAlgorithm::BreadthFirstSearch || selectedAlgorithm == PathfindingAlgorithm::DepthFirstSearch)
                        {
                            cout << "Algorithm selected: " << (selectedAlgorithm == PathfindingAlgorithm::BreadthFirstSearch ? "Breadth-first search" : "Depth-first search") << endl;
                        }

                        isAlgorithmSelectionWindowOpen = false;
                    }
                    window.setView(camera);
                }
                else if (event.mouseButton.button == Mouse::Right)
                {
                    panning = true;
                    m0 = window.mapPixelToCoords(Mouse::getPosition(window));
                }
                break;

            case Event::MouseButtonReleased:
                if (event.mouseButton.button == Mouse::Left)
                {
                    tileStack.clear();
                    editing = false;
                }
                else if (event.mouseButton.button == Mouse::Right)
                {
                    panning = false;
                }
                break;

            case Event::MouseMoved:
                if (editing)
                {
                    coords = window.mapPixelToCoords(Mouse::getPosition(window));
                    tileHover.x = floor(coords.x / board.tileDim);
                    tileHover.y = floor(coords.y / board.tileDim);
                    if (tileHover != tileStack.back())
                    {
                        tileStack.push_back(tileHover);
                        onClick(tileHover);
                    }
                }
                else if (panning)
                {
                    m1 = window.mapPixelToCoords(Mouse::getPosition(window));
                    camera.setCenter(camera.getCenter() + m0 - m1);
                    camera.move(m0 - m1);
                    window.setView(camera);
                    m0 = window.mapPixelToCoords(Mouse::getPosition(window));
                }
                break;

            case Event::MouseWheelMoved:
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
            }
        }

        deltaTime = clock.restart().asSeconds();
        if (!isAlgorithmSelectionWindowOpen && isTimerRunning)
        {
            timer -= deltaTime;
            timerText.setString("Time left: " + std::to_string(static_cast<int>(timer)) + " seconds");

            if (timer <= 0.0f)
            {
                isTimerRunning = false;
                timer = 30.0f;
                timerText.setString("Time left: " + std::to_string(static_cast<int>(timer)) + " seconds");
            }
        }

        window.setView(window.getDefaultView());
        window.draw(editorText);
        window.draw(clearGridButton);
        window.draw(clearGridText);
        window.draw(chooseAlgorithmButton);
        window.draw(chooseAlgorithmText);

        window.draw(timerBox);
        window.draw(timerText);

        window.setView(camera);
        window.draw(this->board);
        window.display();
    }
    return this->board.finish->id % this->board.numCols;
}

string Player::getFileName(RenderWindow& window, Font& font)
{
    // Needs updated UI
    Vector2f winCenter = ((Vector2f)window.getSize()) / 2.0f;
    string query = "Enter File Name (No Extension): ";
    Text fileSelect(query, font, 24);
    fileSelect.setOrigin(fileSelect.getLocalBounds().width / 2.0, fileSelect.getLocalBounds().height / 2.0);
    fileSelect.setFillColor(Color::Black);
    fileSelect.setPosition(winCenter);
    string input;
    while (window.isOpen())
    {
        window.clear(sf::Color(173, 216, 230));
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
    return "map";
}