#ifndef START_SCREEN_H
#define START_SCREEN_H

#include <SFML/Graphics.hpp>
#include <iostream>

#include "menu.h"

using namespace std;
using namespace sf;

class StartScreen
{
public:
    StartScreen(RenderWindow &window);
    void show();
    bool isFinished() const;

private:
    void loadResources();
    void handleEvents();
    void update();
    void render();
    void fadeOut();

    RenderWindow &window;
    Texture backgroundTexture;
    Sprite backgroundSprite;
    Texture logoTexture;
    Sprite logoSprite;
    Clock clock;
    Menu menu;
    bool showMenu;
    bool finished;
    float alpha;
};

StartScreen::StartScreen(RenderWindow &window) : window(window), menu(window, "Player"), showMenu(false), finished(false), alpha(255)
{
    loadResources();
}

void StartScreen::loadResources()
{
    if (!backgroundTexture.loadFromFile("img/menu_background.jpg"))
    {
        cerr << "Error loading background image" << endl;
    }
    else
    {
        backgroundSprite.setTexture(backgroundTexture);
        float scaleX = static_cast<float>(window.getSize().x) / backgroundTexture.getSize().x;
        float scaleY = static_cast<float>(window.getSize().y) / backgroundTexture.getSize().y;
        backgroundSprite.setScale(scaleX, scaleY);
    }

    if (!logoTexture.loadFromFile("img/spaceshooterlogo.png"))
    {
        cerr << "Error loading spaceshooter logo" << endl;
    }
    else
    {
        logoSprite.setTexture(logoTexture);
        logoSprite.setPosition(
            (window.getSize().x - logoSprite.getGlobalBounds().width) / 2,
            (window.getSize().y - logoSprite.getGlobalBounds().height) / 2);
    }
}

void StartScreen::show()
{
    while (window.isOpen() && !finished)
    {
        handleEvents();
        update();
        render();
    }
}

void StartScreen::handleEvents()
{
    Event event;
    while (window.pollEvent(event))
    {
        if (event.type == Event::Closed)
        {
            window.close();
        }
    }
}

void StartScreen::update()
{
    if (clock.getElapsedTime().asSeconds() > 3)
    {
        fadeOut();
    }
}

void StartScreen::fadeOut()
{
    if (alpha > 0)
    {
        alpha -= 5.0f;
        logoSprite.setColor(Color(255, 255, 255, static_cast<Uint8>(alpha)));
    }
    else
    {
        showMenu = true;
        finished = true;
    }
}

void StartScreen::render()
{
    window.clear();
    window.draw(backgroundSprite);
    if (!showMenu)
    {
        window.draw(logoSprite);
    }
    window.display();
}

bool StartScreen::isFinished() const
{
    return finished;
}

#endif
