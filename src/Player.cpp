#include "Player.h"
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <string>
#include <SFML/Window/Keyboard.hpp>

Player::Player(std::string name) : name(name) {}

Player::~Player()
{
    this->board.reset();
}

void Player::onClick(sf::Vector2f pos)
{
    unsigned int col = floor(pos.x / this->board.tileDim);
    unsigned int row = floor(pos.y / this->board.tileDim);
    if ((col < this->board.numCols) && (row < this->board.numRows))
    {
        this->board.updateTile(row, col);
    }
}

void Player::buildBoard(unsigned int numRows, unsigned int numCols)
{
    this->board.build(numRows, numCols);
}

void Player::buildBoard(std::string fileName)
{
    this->board.build(fileName);
}

void Player::resetBoard()
{
    this->board.reset();
}

void Player::editBoard(sf::RenderWindow& window, sf::Font& font)
{
    // Set the initial window position
    sf::Vector2i screenCenter(sf::VideoMode::getDesktopMode().width / 2, sf::VideoMode::getDesktopMode().height / 2);
    sf::Vector2i windowPosition(screenCenter.x - window.getSize().x / 2, screenCenter.y - window.getSize().y / 2);
    window.setPosition(windowPosition);

    bool panning = false;
    bool isClearButtonClicked = false;
    bool isChooseAlgorithmButtonClicked = false;

    // Create the camera view
    sf::View camera(sf::FloatRect(0, 0, 250, 250));
    camera.setCenter(this->board.numCols * 16, this->board.numRows * 16);
    float topLimit = 0.22;
    float bottomLimit = 0.90;
    camera.setViewport(sf::FloatRect(0, topLimit, 1, bottomLimit - topLimit));

    // Set the camera view to the window
    window.setView(camera);

    sf::Vector2f m0;
    sf::Vector2f m1;
    sf::Vector2i pixel;
    std::string fileName;

    const int buttonWidth = 120;
    const int buttonHeight = 30;

    sf::RectangleShape clearGridButton(sf::Vector2f(buttonWidth, buttonHeight));
    clearGridButton.setFillColor(sf::Color(66, 135, 245));
    clearGridButton.setPosition(10, 10);

    sf::Text clearGridText("Clear Grid", font, 18);
    clearGridText.setFillColor(sf::Color::White);
    clearGridText.setPosition(clearGridButton.getPosition().x + (buttonWidth - clearGridText.getLocalBounds().width) / 2,
                              clearGridButton.getPosition().y + (buttonHeight - clearGridText.getLocalBounds().height) / 2);

    sf::RectangleShape chooseAlgorithmButton(sf::Vector2f(buttonWidth + 60, buttonHeight));
    chooseAlgorithmButton.setFillColor(sf::Color(66, 135, 245));
    chooseAlgorithmButton.setPosition(window.getSize().x - buttonWidth - 80, 10);

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

    const float minCameraY = window.getSize().y - timerBoxHeight - 20;

    while (window.isOpen())
    {
        window.clear(sf::Color(173, 216, 230));
        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
            case sf::Event::Closed:
                window.close();
                break;

            case sf::Event::KeyPressed:
                if (event.key.code == sf::Keyboard::Enter)
                {
                    window.setView(window.getDefaultView());
                    fileName = getFileName(window, font);
                    this->board.writeToFile(fileName);
                    return;
                }
                else if (event.key.code == sf::Keyboard::Escape)
                {
                    return;
                }
                break;

            case sf::Event::MouseButtonPressed:
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    pixel = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
                    sf::Vector2f worldPos = window.mapPixelToCoords(pixel);

                    if (clearGridButton.getGlobalBounds().contains(worldPos))
                    {
                        isClearButtonClicked = true;
                    }
                    else if (chooseAlgorithmButton.getGlobalBounds().contains(worldPos))
                    {
                        isChooseAlgorithmButtonClicked = true;
                    }
                }
                else if (event.mouseButton.button == sf::Mouse::Right)
                {
                    panning = true;
                    m0 = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                }
                break;

            case sf::Event::MouseButtonReleased:
                if (event.mouseButton.button == sf::Mouse::Right)
                {
                    panning = false;
                }
                else if (event.mouseButton.button == sf::Mouse::Left)
                {
                    pixel = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
                    sf::Vector2f worldPos = window.mapPixelToCoords(pixel);

                    if (isClearButtonClicked)
                    {
                        resetBoard();
                    }
                    else if (!isChooseAlgorithmButtonClicked)
                    {
                        onClick(worldPos);
                    }

                    isClearButtonClicked = false;
                    isChooseAlgorithmButtonClicked = false;
                }
                break;

            case sf::Event::MouseMoved:
                if (panning)
                {
                    m1 = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                    camera.setCenter(camera.getCenter() + m0 - m1);
                    camera.move(m0 - m1);

                    if (camera.getCenter().y < minCameraY)
                    {
                        camera.setCenter(camera.getCenter().x, minCameraY);
                    }

                    window.setView(camera);
                    m0 = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                }
                break;

            case sf::Event::MouseWheelMoved:
                if (!panning)
                {
                    float zoom = (event.mouseWheel.delta >= 0) ? .5 : 2;
                    pixel = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
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

        float deltaTime = clock.restart().asSeconds();
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

        // Draw elements on the screen
        window.setView(window.getDefaultView());
        sf::Text editorText("Editor Menu Keybindings:\nLeft Click (M1)- Edit Grid Element\nRight Click (M2)- Pan Grid View\n"
                            "Scroll Wheel Up/Down- Zoom In/Out on Hovered Pixel\nEnter- Save Map to File\nEscape- Return to Debug Menu\n", font, 18);
        editorText.setOrigin(editorText.getLocalBounds().width / 2.0, editorText.getLocalBounds().height / 2.0);
        editorText.setFillColor(sf::Color::White);
        editorText.setPosition(editorText.getLocalBounds().width, editorText.getLocalBounds().height - 60);

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

std::string Player::getFileName(sf::RenderWindow& window, sf::Font& font)
{
    sf::Vector2f winCenter = ((sf::Vector2f)window.getSize()) / 2.0f;
    std::string query = "Enter File Name (No Extension): ";
    sf::Text fileSelect(query, font, 24);
    fileSelect.setOrigin(fileSelect.getLocalBounds().width / 2.0, fileSelect.getLocalBounds().height / 2.0);
    fileSelect.setFillColor(sf::Color::Black);
    fileSelect.setPosition(winCenter);
    std::string input;
    while (window.isOpen())
    {
        window.clear(sf::Color(173, 216, 230)); // Light blue-green background
        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
            case sf::Event::Closed:
                window.close();
                break;

            case sf::Event::KeyPressed:
                if (event.key.code == sf::Keyboard::Enter)
                {
                    return input;
                }
                else if ((event.key.code == sf::Keyboard::Backspace) && (input.size() > 0))
                {
                    input = input.substr(0, input.size() - 1);
                    fileSelect.setString(query + input);
                }
                break;
            case sf::Event::TextEntered:
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
