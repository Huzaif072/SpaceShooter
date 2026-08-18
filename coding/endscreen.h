#ifndef ENDSCREEN_H
#define ENDSCREEN_H

#include <SFML/Graphics.hpp>
#include <fstream>
#include <algorithm>
#include <string>
#include <vector>
using namespace sf;
using namespace std;

class EndScreen
{
public:
    // isWin = true when all 3 levels complete; otherwise level-complete between levels
    EndScreen(RenderWindow &window, Font &font, int finalScore,
              const string &playerName, const Sprite &bg, bool isWin)
        : window(window), font(font), finalScore(finalScore),
          playerName(playerName), bg(bg), isWin(isWin) {}

    // Returns 0 = continue / play again,  1 = main menu
    int show()
    {
        auto makeText = [&](const string &str, unsigned size, Color col, float y) {
            Text t;
            t.setFont(font);
            t.setString(str);
            t.setCharacterSize(size);
            t.setFillColor(col);
            t.setStyle(Text::Bold);
            t.setPosition(window.getSize().x / 2.f - t.getGlobalBounds().width / 2.f, y);
            return t;
        };

        float cy = window.getSize().y / 2.f;

        Text title  = makeText(isWin ? "YOU WIN!" : "Level Complete!", 72,
                               isWin ? Color::Yellow : Color(0, 220, 100), cy - 220);
        Text sText  = makeText("Score: " + to_string(finalScore), 40, Color::White, cy - 130);
        Text opt0   = makeText(isWin ? "Play Again" : "Next Level", 34, Color::White, cy - 10);
        Text opt1   = makeText("Main Menu", 34, Color::White, cy + 60);

        vector<Text*> opts = { &opt0, &opt1 };
        int selected = 0;

        while (window.isOpen())
        {
            Event event;
            while (window.pollEvent(event))
            {
                if (event.type == Event::Closed) { window.close(); return 1; }
                if (event.type == Event::KeyPressed)
                {
                    if (event.key.code == Keyboard::Up || event.key.code == Keyboard::Down)
                        selected ^= 1;
                    if (event.key.code == Keyboard::Enter)
                        return selected;
                }
            }

            opts[0]->setFillColor(selected == 0 ? Color::Yellow : Color::White);
            opts[1]->setFillColor(selected == 1 ? Color::Yellow : Color::White);

            window.clear();
            window.draw(bg);
            window.draw(title);
            window.draw(sText);
            for (auto *t : opts) window.draw(*t);
            window.display();
        }
        return 1;
    }

private:
    RenderWindow &window;
    Font         &font;
    int           finalScore;
    string        playerName;
    const Sprite &bg;
    bool          isWin;
};

#endif // ENDSCREEN_H
