#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include "Player.h"

using namespace std;
using namespace sf;

string querySelectorDEBUG(RenderWindow& window, Font& font, string query);

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

    RenderWindow window(VideoMode(monitorWidth / 2, monitorHeight / 2), "Trace Race", Style::Default);

    // Set the initial window position to the center of the screen
    sf::Vector2i screenCenter(monitorWidth / 2, monitorHeight / 2);
    sf::Vector2i windowPosition(screenCenter.x - window.getSize().x / 2, screenCenter.y - window.getSize().y / 2);
    window.setPosition(windowPosition);

    window.setFramerateLimit(360);

    Vector2f winCenter = ((Vector2f)window.getSize())/2.0f;
    Text debugText("Debug Menu Keybindings:\nd- Set Dimensions\ne- Enter Edit Mode (Dimension Input)\n"
                "f- Enter Edit Mode (File Input)\n", font, 24);
    debugText.setOrigin(debugText.getLocalBounds().width/2.0, debugText.getLocalBounds().height/2.0);
    debugText.setFillColor(Color::Black);
    debugText.setPosition(winCenter);

    Player* p1 = new Player("Debug");

    while (window.isOpen())
    {
        window.clear(Color::Blue);
        Event event;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
                case Event::Closed:
                    window.close();
                    break;

                case Event::KeyPressed:
                    if (event.key.code == Keyboard::D)
                    {
                        // Set Dimensions
                        window.pollEvent(event);
                        dims = querySelectorDEBUG(window, font, "Enter Dimensions (RxC): ");
                        numRows = stoi(dims.substr(0, dims.find('x')));
                        numCols = stoi(dims.substr(dims.find('x')+1));                        
                    }
                    else if (event.key.code == Keyboard::E)
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
        window.draw(debugText);
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