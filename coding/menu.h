#ifndef MENU_H
#define MENU_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <fstream>

using namespace sf;
using namespace std;

class Menu
{
public:
    Menu(RenderWindow &window, const string &playerName);
    bool run();

private:
    void loadResources();
    void handleEvents();
    void render();
    void displayInstructions();
    void displayHighScores();
    void centerText(Text &text, float x, float y);
    void fadeOut();
    void fadeIn();
    void updateSelection(int change);

    RenderWindow &window;
    Font font;
    Text playerNameText;
    Text playText;
    Text instructionsText;
    Text highScoresText;
    Text quitText;
    vector<pair<string, int>> highScores;
    bool showInstructions = false;
    bool showHighScores = false;
    bool fadeOutEffect = false;
    bool fadeInEffect = false;
    float alpha = 255;
    string playerName;
    Texture backgroundTexture;
    Sprite backgroundSprite;
    Texture logoTexture;
    Sprite logoSprite;
    RectangleShape playBox;
    RectangleShape instructionsBox;
    RectangleShape highScoresBox;
    RectangleShape quitBox;
    int selectedOption = 0;

    enum class NextAction
    {
        None,
        Play,
        Instructions,
        HighScores,
        Quit,
        BackToMenu
    } nextAction = NextAction::None;
};

Menu::Menu(RenderWindow &window, const string &playerName) : window(window), playerName(playerName)
{
    loadResources();
    backgroundSprite.setTexture(backgroundTexture);
    backgroundSprite.setScale(
        static_cast<float>(window.getSize().x) / backgroundTexture.getSize().x,
        static_cast<float>(window.getSize().y) / backgroundTexture.getSize().y);
    logoSprite.setTexture(logoTexture);
    logoSprite.setPosition(window.getSize().x / 2.0f - logoSprite.getGlobalBounds().width / 2.0f, -175);
}

void Menu::loadResources()
{
    if (!backgroundTexture.loadFromFile("img/menu_background.jpg"))
    {
        cerr << "Error loading background image" << endl;
    }

    if (!logoTexture.loadFromFile("img/spaceshooterlogo.png"))
    {
        cerr << "Error loading logo image" << endl;
    }

    if (!font.loadFromFile("Fonts/ariblk.ttf"))
    {
        cerr << "Error loading menu screen font" << endl;
    }

    playerNameText.setFont(font);
    playerNameText.setString("Hello, " + playerName + "!");
    playerNameText.setCharacterSize(40);
    centerText(playerNameText, window.getSize().x / 2.0f, 510);

    playText.setFont(font);
    playText.setString("PLAY");
    playText.setCharacterSize(24);
    centerText(playText, window.getSize().x / 2.0f, 610);

    instructionsText.setFont(font);
    instructionsText.setString("Instructions");
    instructionsText.setCharacterSize(24);
    centerText(instructionsText, window.getSize().x / 2.0f, 670);

    highScoresText.setFont(font);
    highScoresText.setString("High Scores");
    highScoresText.setCharacterSize(24);
    centerText(highScoresText, window.getSize().x / 2.0f, 730);

    quitText.setFont(font);
    quitText.setString("Quit");
    quitText.setCharacterSize(24);
    centerText(quitText, window.getSize().x / 2.0f, 790);

    float boxWidth = 300.0f;
    float boxHeight = 50.0f;
    float boxX = window.getSize().x / 2.0f - boxWidth / 2.0f;

    playBox.setSize(Vector2f(boxWidth, boxHeight));
    playBox.setFillColor(Color(75, 0, 130, 200));
    playBox.setOutlineThickness(2);
    playBox.setOutlineColor(Color::White);
    playBox.setPosition(boxX, 590);

    instructionsBox.setSize(Vector2f(boxWidth, boxHeight));
    instructionsBox.setFillColor(Color(75, 0, 130, 200));
    instructionsBox.setOutlineThickness(2);
    instructionsBox.setOutlineColor(Color::White);
    instructionsBox.setPosition(boxX, 650);

    highScoresBox.setSize(Vector2f(boxWidth, boxHeight));
    highScoresBox.setFillColor(Color(75, 0, 130, 200));
    highScoresBox.setOutlineThickness(2);
    highScoresBox.setOutlineColor(Color::White);
    highScoresBox.setPosition(boxX, 710);

    quitBox.setSize(Vector2f(boxWidth, boxHeight));
    quitBox.setFillColor(Color(75, 0, 130, 200));
    quitBox.setOutlineThickness(2);
    quitBox.setOutlineColor(Color::White);
    quitBox.setPosition(boxX, 770);

    ifstream file("highscores.txt");
    string name;
    int score;
    while (file >> name >> score)
    {
        highScores.emplace_back(name, score);
    }

    sort(highScores.begin(), highScores.end(), [](const pair<string, int> &a, const pair<string, int> &b)
         { return a.second > b.second; });
}

void Menu::centerText(Text &text, float x, float y)
{
    text.setPosition(x - text.getGlobalBounds().width / 2.0f, y - text.getGlobalBounds().height / 2.0f);
}

bool Menu::run()
{
    while (window.isOpen())
    {
        handleEvents();
        render();
        if (nextAction == NextAction::Play)
        {
            return true;
        }
    }
    return false;
}


void Menu::handleEvents()
{
    Event event;
    while (window.pollEvent(event))
    {
        if (event.type == Event::Closed)
        {
            window.close();
        }
        if (event.type == Event::MouseButtonPressed)
        {
            Vector2i mousePos = Mouse::getPosition(window);
            if (playBox.getGlobalBounds().contains(mousePos.x, mousePos.y))
            {
                fadeOutEffect = true;
                nextAction = NextAction::Play;
            }
            else if (instructionsBox.getGlobalBounds().contains(mousePos.x, mousePos.y))
            {
                fadeOutEffect = true;
                nextAction = NextAction::Instructions;
            }
            else if (highScoresBox.getGlobalBounds().contains(mousePos.x, mousePos.y))
            {
                fadeOutEffect = true;
                nextAction = NextAction::HighScores;
            }
            else if (quitBox.getGlobalBounds().contains(mousePos.x, mousePos.y))
            {
                fadeOutEffect = true;
                nextAction = NextAction::Quit;
            }
        }
        if (event.type == Event::KeyPressed)
        {
            if (showInstructions || showHighScores)
            {
                if (event.key.code == Keyboard::Escape || event.key.code == Keyboard::Backspace)
                {
                    fadeOutEffect = true;
                    nextAction = NextAction::BackToMenu;
                }
            }
            else
            {
                if (event.key.code == Keyboard::Up)
                {
                    updateSelection(-1);
                }
                else if (event.key.code == Keyboard::Down)
                {
                    updateSelection(1);
                }
                else if (event.key.code == Keyboard::Enter)
                {
                    switch (selectedOption)
                    {
                    case 0:
                        fadeOutEffect = true;
                        nextAction = NextAction::Play;
                        break;
                    case 1:
                        fadeOutEffect = true;
                        nextAction = NextAction::Instructions;
                        break;
                    case 2:
                        fadeOutEffect = true;
                        nextAction = NextAction::HighScores;
                        break;
                    case 3:
                        fadeOutEffect = true;
                        nextAction = NextAction::Quit;
                        break;
                    }
                }
            }
        }
    }
}

void Menu::updateSelection(int change)
{
    selectedOption = (selectedOption + change + 4) % 4;
}

void Menu::render()
{
    window.clear();
    window.draw(backgroundSprite);
    window.draw(logoSprite);

    if (!showInstructions && !showHighScores)
    {
        window.draw(playerNameText);

        window.draw(playBox);
        window.draw(instructionsBox);
        window.draw(highScoresBox);
        window.draw(quitBox);

        window.draw(playText);
        window.draw(instructionsText);
        window.draw(highScoresText);
        window.draw(quitText);

        switch (selectedOption)
        {
        case 0:
            playText.setFillColor(Color::Yellow);
            instructionsText.setFillColor(Color::White);
            highScoresText.setFillColor(Color::White);
            quitText.setFillColor(Color::White);
            break;
        case 1:
            playText.setFillColor(Color::White);
            instructionsText.setFillColor(Color::Yellow);
            highScoresText.setFillColor(Color::White);
            quitText.setFillColor(Color::White);
            break;
        case 2:
            playText.setFillColor(Color::White);
            instructionsText.setFillColor(Color::White);
            highScoresText.setFillColor(Color::Yellow);
            quitText.setFillColor(Color::White);
            break;
        case 3:
            playText.setFillColor(Color::White);
            instructionsText.setFillColor(Color::White);
            highScoresText.setFillColor(Color::White);
            quitText.setFillColor(Color::Yellow);
            break;
        }
    }

    if (showInstructions)
    {
        displayInstructions();
    }
    else if (showHighScores)
    {
        displayHighScores();
    }

    window.display();

    if (fadeOutEffect)
    {
        fadeOut();
    }
    else if (fadeInEffect)
    {
        fadeIn();
    }
}

void Menu::fadeOut()
{
    if (alpha > 0)
    {
        alpha -= 5.0f;
        if (alpha < 0)
            alpha = 0;

        if (!showInstructions && !showHighScores)
        {
            playerNameText.setFillColor(Color(255, 255, 255, static_cast<Uint8>(alpha)));
            playText.setFillColor(Color(255, 255, 255, static_cast<Uint8>(alpha)));
            instructionsText.setFillColor(Color(255, 255, 255, static_cast<Uint8>(alpha)));
            highScoresText.setFillColor(Color(255, 255, 255, static_cast<Uint8>(alpha)));
            quitText.setFillColor(Color(255, 255, 255, static_cast<Uint8>(alpha)));

            playBox.setFillColor(Color(75, 0, 130, static_cast<Uint8>(alpha)));
            instructionsBox.setFillColor(Color(75, 0, 130, static_cast<Uint8>(alpha)));
            highScoresBox.setFillColor(Color(75, 0, 130, static_cast<Uint8>(alpha)));
            quitBox.setFillColor(Color(75, 0, 130, static_cast<Uint8>(alpha)));
        }

        if (showInstructions)
        {
            displayInstructions();
        }
        else if (showHighScores)
        {
            displayHighScores();
        }
    }
    else
    {
        fadeOutEffect = false;
        alpha = 255;

        switch (nextAction)
        {
        case NextAction::Play:
            return;
            // Start the game
            break;
        case NextAction::Instructions:
            fadeInEffect = true;
            showInstructions = true;
            showHighScores = false;
            break;
        case NextAction::HighScores:
            fadeInEffect = true;
            showHighScores = true;
            showInstructions = false;
            break;
        case NextAction::Quit:
            window.close();
            break;
        case NextAction::BackToMenu:
            fadeInEffect = true;
            showInstructions = false;
            showHighScores = false;
            playerNameText.setFillColor(Color::White);
            playText.setFillColor(Color::White);
            instructionsText.setFillColor(Color::White);
            highScoresText.setFillColor(Color::White);
            quitText.setFillColor(Color::White);

            playBox.setFillColor(Color(75, 0, 130, 200));
            instructionsBox.setFillColor(Color(75, 0, 130, 200));
            highScoresBox.setFillColor(Color(75, 0, 130, 200));
            quitBox.setFillColor(Color(75, 0, 130, 200));
            break;
        case NextAction::None:
            break;
        }
        nextAction = NextAction::None;
    }
}

void Menu::fadeIn()
{
    if (alpha < 255)
    {
        alpha += 5.0f;
        if (alpha > 255)
            alpha = 255;

        if (!showInstructions && !showHighScores)
        {
            playerNameText.setFillColor(Color(255, 255, 255, static_cast<Uint8>(alpha)));
            playText.setFillColor(Color(255, 255, 255, static_cast<Uint8>(alpha)));
            instructionsText.setFillColor(Color(255, 255, 255, static_cast<Uint8>(alpha)));
            highScoresText.setFillColor(Color(255, 255, 255, static_cast<Uint8>(alpha)));
            quitText.setFillColor(Color(255, 255, 255, static_cast<Uint8>(alpha)));

            playBox.setFillColor(Color(75, 0, 130, static_cast<Uint8>(alpha)));
            instructionsBox.setFillColor(Color(75, 0, 130, static_cast<Uint8>(alpha)));
            highScoresBox.setFillColor(Color(75, 0, 130, static_cast<Uint8>(alpha)));
            quitBox.setFillColor(Color(75, 0, 130, static_cast<Uint8>(alpha)));
        }

        if (showInstructions)
        {
            displayInstructions();
        }
        else if (showHighScores)
        {
            displayHighScores();
        }
    }
    else
    {
        fadeInEffect = false;
        alpha = 255;
    }
}

void Menu::displayInstructions()
{
    Text instructions;
    instructions.setFont(font);
    instructions.setString("Instructions:\n\n1. Use arrow keys to move your spaceship.\n"
                           "2. Press spacebar to shoot.\n"
                           "3. Avoid enemy fire and obstacles.\n"
                           "4. Destroy enemy ships to score points.\n"
                           "5. Collect power-ups to enhance your abilities.");
    instructions.setCharacterSize(24);
    instructions.setFillColor(Color(255, 255, 255, static_cast<Uint8>(alpha)));
    centerText(instructions, window.getSize().x / 2.0f, 600);
    window.draw(instructions);
}

void Menu::displayHighScores()
{
    Text highScoreTitle;
    highScoreTitle.setFont(font);
    highScoreTitle.setString("High Scores:");
    highScoreTitle.setCharacterSize(24);
    centerText(highScoreTitle, window.getSize().x / 2.0f, 500);
    window.draw(highScoreTitle);

    float yOffset = 200;
    for (const auto &entry : highScores)
    {
        Text scoreText;
        scoreText.setFont(font);
        scoreText.setString(entry.first + ": " + to_string(entry.second));
        scoreText.setCharacterSize(24);
        scoreText.setFillColor(Color(255, 255, 255, static_cast<Uint8>(alpha)));
        centerText(scoreText, window.getSize().x / 2.0f, yOffset);
        window.draw(scoreText);
        yOffset += 30;
    }
}

#endif // MENU_H