#include "Player.h"
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <string>
#include <SFML/Window/Keyboard.hpp>
#include <iostream>

using namespace std;
using namespace sf;

Player::Player(string name) : name(name){}

Player::~Player()
{
    this->board.reset();
}

void Player::onClick(Vector2i pos)
{
    if ((pos.x < this->board.numCols) && (pos.y < this->board.numRows))
    {
        this->board.updateTile(pos.y, pos.x);
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

void Player::resetBoard(bool wall)
{
    this->board.resetTiles(wall);
}

void Player::editBoard(RenderWindow& window, Font& font)
{
//    sf::Vector2i screenCenter(sf::VideoMode::getDesktopMode().width / 2, sf::VideoMode::getDesktopMode().height / 2);
//    sf::Vector2i windowPosition(screenCenter.x - window.getSize().x / 2, screenCenter.y - window.getSize().y / 2);
//    window.setPosition(windowPosition);
    bool panning = false;
    bool editing = false;
    const std::vector<std::string> dropdownOptions = { "Option 1", "Option 2" };
    int selectedOption = -1; // Default to no option selected
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
    clearGridButton.setPosition(10, 10); // Moved the button down by 10 units.

    sf::Text clearGridText("Clear Grid", font, 18);
    clearGridText.setFillColor(sf::Color::White);
    clearGridText.setPosition(clearGridButton.getPosition().x + (buttonWidth - clearGridText.getLocalBounds().width) / 2,
                              clearGridButton.getPosition().y + (buttonHeight - clearGridText.getLocalBounds().height) / 2);

    sf::RectangleShape chooseAlgorithmButton(sf::Vector2f(buttonWidth + 60, buttonHeight));
    chooseAlgorithmButton.setFillColor(sf::Color(66, 135, 245));
    chooseAlgorithmButton.setPosition(window.getSize().x - buttonWidth - 80, clearGridButton.getPosition().y); // Align the "Choose Algorithm" button with the "Clear Grid" button

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

    Text editorText("Editor Menu Keybindings:\nLeft Click (M1)- Edit Grid Element\nRight Click (M2)- Pan Grid View\n"
                    "Scroll Wheel Up/Down- Zoom In/Out on Hovered Pixel\nEnter- Save Map to File\nEscape- Return to Debug Menu\n", font, 18);
    editorText.setOrigin(editorText.getLocalBounds().width/2.0, editorText.getLocalBounds().height/2.0);
    editorText.setFillColor(Color::White);
    editorText.setPosition(editorText.getLocalBounds().width, editorText.getLocalBounds().height - 20);

    const float minCameraY = window.getSize().y - timerBoxHeight - 20;
    const int dropdownButtonHeight = 30;
    bool isDropdownVisible = false;

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
                        return;
                    }
                    else if (event.key.code == Keyboard::G)
                    {
                        this->board.generate(15, 15);
                    }
                    
                    else if (event.key.code == Keyboard::Escape)
                    {
                        return;
                    }
                    break;

                case Event::MouseButtonPressed:
                    if (event.mouseButton.button == Mouse::Left)
                    {
                        editing = true;
                        coords = window.mapPixelToCoords(Vector2i(event.mouseButton.x, event.mouseButton.y));
                        tileHover.x = floor(coords.x/board.tileDim);
                        tileHover.y = floor(coords.y/board.tileDim);
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
                            cout << "out" << endl;

                            if (!isDropdownVisible)
                            {
                                isDropdownVisible = true;
                            }
                            else
                            {
                                // Process the selected option here (you can use 'selectedOption' variable)
                                for (size_t i = 0; i < dropdownOptions.size(); ++i)
                                {
                                    sf::FloatRect optionArea(chooseAlgorithmButton.getPosition().x, chooseAlgorithmButton.getPosition().y + dropdownButtonHeight + i * buttonHeight, buttonWidth + 60, buttonHeight);
                                    if (optionArea.contains(coords))
                                    {
                                        selectedOption = static_cast<int>(i);
                                        std::cout << "Selected Option: " << dropdownOptions[selectedOption] << std::endl; // For debugging
                                        break;
                                    }
                                }
                            }
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
                        tileHover.x = floor(coords.x/board.tileDim);
                        tileHover.y = floor(coords.y/board.tileDim);
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
                        camera.move(m0-m1);
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
                        camera.move(m0-m1);
                        window.setView(camera);
                    }
                    break;
            }
        }

        deltaTime = clock.restart().asSeconds();
        if (isTimerRunning)
        {
            timer -= deltaTime;
            timerText.setString("Time left: " + std::to_string(static_cast<int>(timer)) + " seconds");

            if (timer <= 0.0f)
            {
                isTimerRunning = false;
                timer = 30.0f;
            }
        }
        
        if (isDropdownVisible)
        {
            sf::RectangleShape dropdownBackground(sf::Vector2f(buttonWidth + 60, buttonHeight * dropdownOptions.size()));
            dropdownBackground.setFillColor(sf::Color(66, 135, 245));
            dropdownBackground.setPosition(chooseAlgorithmButton.getPosition().x, chooseAlgorithmButton.getPosition().y + dropdownButtonHeight);

            for (size_t i = 0; i < dropdownOptions.size(); ++i)
            {
                sf::Text optionText(dropdownOptions[i], font, 18);
                optionText.setFillColor(sf::Color::White);
                optionText.setPosition(dropdownBackground.getPosition().x + 30, dropdownBackground.getPosition().y + i * buttonHeight);
                window.draw(optionText);
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
    
}

string Player::getFileName(RenderWindow& window, Font& font)
{
    // Needs updated UI
    Vector2f winCenter = ((Vector2f)window.getSize())/2.0f;
    string query = "Enter File Name (No Extension): ";
    Text fileSelect(query, font, 24);
    fileSelect.setOrigin(fileSelect.getLocalBounds().width/2.0, fileSelect.getLocalBounds().height/2.0);
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
    return "map";
}