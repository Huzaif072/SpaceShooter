#ifndef PAUSE_MENU_H
#define PAUSE_MENU_H

#include <SFML/Graphics.hpp>
#include <iostream>

using namespace sf;
using namespace std;

class PauseMenu
{
public:
    PauseMenu(RenderWindow &window, Sprite &background);
    void show();
    int getSelectedOption();

private:
    RenderWindow &window;
    Sprite &background;
    Font font;
    vector<Text> options;
    int selectedOption;
    void draw();
};

PauseMenu::PauseMenu(RenderWindow &window, Sprite &background) : window(window), background(background), selectedOption(0)
{
    if (!font.loadFromFile("Fonts/ariblk.ttf"))
    {
        cerr << "Error loading font" << endl;
    }

    Text resumeOption;
    resumeOption.setFont(font);
    resumeOption.setCharacterSize(30);
    resumeOption.setFillColor(Color::White);
    resumeOption.setString("Resume Game");
    resumeOption.setPosition(window.getSize().x / 2 - resumeOption.getGlobalBounds().width / 2, window.getSize().y / 2 - 50);
    options.push_back(resumeOption);

    Text mainMenuOption;
    mainMenuOption.setFont(font);
    mainMenuOption.setCharacterSize(30);
    mainMenuOption.setFillColor(Color::White);
    mainMenuOption.setString("Return to Main Menu");
    mainMenuOption.setPosition(window.getSize().x / 2 - mainMenuOption.getGlobalBounds().width / 2, window.getSize().y / 2);
    options.push_back(mainMenuOption);

    Text exitOption;
    exitOption.setFont(font);
    exitOption.setCharacterSize(30);
    exitOption.setFillColor(Color::White);
    exitOption.setString("Exit Game");
    exitOption.setPosition(window.getSize().x / 2 - exitOption.getGlobalBounds().width / 2, window.getSize().y / 2 + 50);
    options.push_back(exitOption);
}

void PauseMenu::show()
{
    while (true)
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::KeyPressed)
            {
                if (event.key.code == Keyboard::Up)
                {
                    selectedOption = (selectedOption + options.size() - 1) % options.size();
                }
                else if (event.key.code == Keyboard::Down)
                {
                    selectedOption = (selectedOption + 1) % options.size();
                }
                else if (event.key.code == Keyboard::Enter)
                {
                    return;
                }
            }
            else if (event.type == Event::Closed)
            {
                window.close();
                return;
            }
        }
        draw();
    }
}

int PauseMenu::getSelectedOption()
{
    return selectedOption;
}

void PauseMenu::draw()
{
    window.clear();
    window.draw(background); // Draw the background first

    for (size_t i = 0; i < options.size(); ++i)
    {
        if (i == selectedOption)
        {
            options[i].setFillColor(Color::Yellow);
        }
        else
        {
            options[i].setFillColor(Color::White);
        }
        window.draw(options[i]);
    }

    window.display();
}

#endif // PAUSE_MENU_H
