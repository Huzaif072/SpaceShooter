#ifndef NAME_ENTRY_SCREEN_H
#define NAME_ENTRY_SCREEN_H

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <iostream>

using namespace std;
using namespace sf;

class NameEntryScreen
{
public:
    NameEntryScreen(RenderWindow &window, const Texture &backgroundTexture, const Sprite &backgroundSprite);
    void show();
    string getPlayerName() const { return playerName; }
    bool isFinished() const { return finished; }

private:
    void handleEvents();
    void render();
    void update();
    void fadeOut();

    RenderWindow &window;
    const Texture &backgroundTexture;
    const Sprite &backgroundSprite;
    Font font;
    Text promptText;
    Text nameText;
    Text enterButtonText;
    RectangleShape enterButton;
    RectangleShape backdropBox;
    RectangleShape nameInputBox;
    string playerName;
    bool nameEntered;
    bool finished;
    float alpha;
};

NameEntryScreen::NameEntryScreen(RenderWindow &window, const Texture &backgroundTexture, const Sprite &backgroundSprite)
    : window(window), backgroundTexture(backgroundTexture), backgroundSprite(backgroundSprite), nameEntered(false), finished(false), alpha(255)
{
    if (!font.loadFromFile("Fonts/AldotheApache.ttf"))
    {
        cerr << "Error loading name entry screen font" << endl;
    }

    promptText.setFont(font);
    promptText.setString("Enter your name:");
    promptText.setCharacterSize(36);
    promptText.setFillColor(Color::White);
    promptText.setStyle(Text::Bold);

    nameText.setFont(font);
    nameText.setString("");
    nameText.setCharacterSize(24);
    nameText.setFillColor(Color::White);
    nameText.setStyle(Text::Bold);

    enterButton.setSize(Vector2f(100, 50));
    enterButton.setFillColor(Color::Magenta);
    enterButton.setOutlineColor(Color::White);
    enterButton.setOutlineThickness(2);

    enterButtonText.setFont(font);
    enterButtonText.setString("OK");
    enterButtonText.setCharacterSize(24);
    enterButtonText.setFillColor(Color::White);
    enterButtonText.setStyle(Text::Bold);

    backdropBox.setSize(Vector2f(600, 300));
    backdropBox.setFillColor(Color(75, 0, 130, 200));
    backdropBox.setOutlineColor(Color(153, 50, 204));
    backdropBox.setOutlineThickness(5);
    backdropBox.setOrigin(backdropBox.getSize().x / 2, backdropBox.getSize().y / 2);

    nameInputBox.setSize(Vector2f(400, 50));
    nameInputBox.setFillColor(Color(75, 0, 130, 255));
    nameInputBox.setOutlineColor(Color::White);
    nameInputBox.setOutlineThickness(2);
    nameInputBox.setOrigin(nameInputBox.getSize().x / 2, nameInputBox.getSize().y / 2);
}

void NameEntryScreen::show()
{
    while (window.isOpen() && !finished)
    {
        handleEvents();
        update();
        render();
    }
}

void NameEntryScreen::handleEvents()
{
    Event event;
    while (window.pollEvent(event))
    {
        if (event.type == Event::Closed)
        {
            window.close();
        }
        if (!nameEntered)
        {
            if (event.type == Event::TextEntered)
            {
                if (event.text.unicode == '\b' && !playerName.empty())
                {
                    playerName.pop_back();
                }
                else if (event.text.unicode >= 32 && event.text.unicode <= 126)
                {
                    playerName += static_cast<char>(event.text.unicode);
                }
                nameText.setString(playerName);
            }
            if (event.type == Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == Mouse::Left)
                {
                    Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
                    if (nameInputBox.getGlobalBounds().contains(mousePos) && playerName.empty())
                    {
                        // Disable clicking if name is empty
                    }
                    else if (enterButton.getGlobalBounds().contains(mousePos) && !playerName.empty())
                    {
                        nameEntered = true;
                    }
                }
            }
            if (event.type == Event::KeyPressed)
            {
                if (event.key.code == Keyboard::Enter && !playerName.empty())
                {
                    nameEntered = true;
                }
            }
        }
    }
}

void NameEntryScreen::update()
{
    if (nameEntered)
    {
        fadeOut();
    }
}

void NameEntryScreen::fadeOut()
{
    if (alpha > 0)
    {
        alpha -= 5.0f;
        if (alpha < 0)
            alpha = 0; // Ensure alpha doesn't go negative

        Color fadeColor(255, 255, 255, static_cast<Uint8>(alpha));
        Color fadeColorBackdrop(75, 0, 130, static_cast<Uint8>(alpha));

        promptText.setFillColor(fadeColor);
        nameText.setFillColor(fadeColor);
        enterButton.setFillColor(fadeColorBackdrop);
        enterButtonText.setFillColor(fadeColor);
        backdropBox.setFillColor(fadeColorBackdrop);
        nameInputBox.setFillColor(fadeColorBackdrop);

        // Also update outline colors if necessary
        backdropBox.setOutlineColor(Color(153, 50, 204, static_cast<Uint8>(alpha)));
        nameInputBox.setOutlineColor(Color(255, 255, 255, static_cast<Uint8>(alpha)));
        enterButton.setOutlineColor(Color(255, 255, 255, static_cast<Uint8>(alpha)));
    }
    else
    {
        finished = true;
    }
}

void NameEntryScreen::render()
{
    window.clear();
    window.draw(backgroundSprite);

    Vector2f center(window.getSize().x / 2.0f, window.getSize().y / 2.0f);

    backdropBox.setPosition(center);
    promptText.setPosition(center.x - promptText.getGlobalBounds().width / 2, center.y - 120);
    nameInputBox.setPosition(center.x, center.y - 40);
    nameText.setPosition(nameInputBox.getPosition().x - nameText.getGlobalBounds().width / 2, nameInputBox.getPosition().y - nameText.getGlobalBounds().height / 2);

    enterButton.setPosition(center.x - enterButton.getSize().x / 2, center.y + 40);
    enterButtonText.setPosition(
        enterButton.getPosition().x + (enterButton.getSize().x - enterButtonText.getGlobalBounds().width) / 2,
        enterButton.getPosition().y + (enterButton.getSize().y - enterButtonText.getGlobalBounds().height) / 2);

    window.draw(backdropBox);
    window.draw(promptText);
    window.draw(nameInputBox);
    window.draw(nameText);
    window.draw(enterButton);
    window.draw(enterButtonText);

    window.display();
}

#endif // NAME_ENTRY_SCREEN_H
