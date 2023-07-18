#include "Player.h"

using namespace std;
using namespace sf;

Player::Player(string name) : name(name){}

Player::~Player()
{
    this->board.reset();
}

void Player::onClick(Vector2f pos)
{
    unsigned int col = floor(pos.x/this->board.tileDim);
    unsigned int row = floor(pos.y/this->board.tileDim);
    if ((col < this->board.numCols) && (row < this->board.numRows))
    {
        this->board.updateTile(row, col);
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

void Player::editBoard(RenderWindow& window, Font& font)
{
    // Needs updated UI
    bool panning = false;
    View camera(FloatRect(0, 0, 250, 250));
    camera.setCenter(this->board.numCols*16, this->board.numRows*16);
    camera.setViewport(FloatRect(0, .25, 1, 1));
    window.setView(camera);
    Vector2f m0;
    Vector2f m1;
    Vector2i pixel;
    string fileName;

    Text editorText("Editor Menu Keybindings:\nLeft Click (M1)- Edit Grid Element\nRight Click (M2)- Pan Grid View\n"
                    "Scroll Wheel Up/Down- Zoom In/Out on Hovered Pixel\nEnter- Save Map to File\nEscape- Return to Debug Menu\n", font, 18);
    editorText.setOrigin(editorText.getLocalBounds().width/2.0, editorText.getLocalBounds().height/2.0);
    editorText.setFillColor(Color::White);
    editorText.setPosition(editorText.getLocalBounds().width, editorText.getLocalBounds().height);

    while (window.isOpen())
    {
        window.clear(Color::Black);
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
                    else if (event.key.code == Keyboard::Escape)
                    {
                        return;
                    }
                    break;

                case Event::MouseButtonPressed:
                    if (event.mouseButton.button == Mouse::Left)
                    {
                        pixel = Vector2i(event.mouseButton.x, event.mouseButton.y);
                        onClick(window.mapPixelToCoords(pixel));
                    }
                    else if (event.mouseButton.button == Mouse::Right)
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
                    if (panning)
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
        window.setView(window.getDefaultView());
        window.draw(editorText);
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
    return "map";
}