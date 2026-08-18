#ifndef GAMEOVER_H
#define GAMEOVER_H

#include <SFML/Graphics.hpp>
#include <fstream>
#include <algorithm>
#include <string>
#include <vector>
using namespace sf;
using namespace std;

class GameOverScreen
{
public:
    GameOverScreen(RenderWindow &window, Font &font, int finalScore,
                   const string &playerName, const Sprite &bg)
        : window(window), font(font), finalScore(finalScore),
          playerName(playerName), bg(bg) {}

    void saveScore()
    {
        vector<pair<string, int>> scores;
        ifstream in("highscores.txt");
        string n; int s;
        while (in >> n >> s) scores.emplace_back(n, s);
        in.close();

        // Update existing or add new
        bool found = false;
        for (auto &entry : scores)
        {
            if (entry.first == playerName)
            {
                entry.second = max(entry.second, finalScore);
                found = true; break;
            }
        }
        if (!found) scores.emplace_back(playerName, finalScore);

        sort(scores.begin(), scores.end(),
             [](const pair<string,int> &a, const pair<string,int> &b){ return a.second > b.second; });

        ofstream out("highscores.txt");
        for (auto &[name, sc] : scores)
            out << name << " " << sc << "\n";
    }

    // Returns: 0 = Play Again, 1 = Main Menu, 2 = Quit
    int show()
    {
        saveScore();

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

        float cx = window.getSize().x / 2.f;
        float cy = window.getSize().y / 2.f;

        Text title  = makeText("GAME OVER", 80, Color::Red,    cy - 240);
        Text sText  = makeText("Score: " + to_string(finalScore), 40, Color::White, cy - 140);
        Text opt0   = makeText("Play Again", 34, Color::White, cy - 20);
        Text opt1   = makeText("Main Menu",  34, Color::White, cy + 50);
        Text opt2   = makeText("Quit",       34, Color::White, cy + 120);

        vector<Text*> opts = { &opt0, &opt1, &opt2 };
        int selected = 0;

        while (window.isOpen())
        {
            Event event;
            while (window.pollEvent(event))
            {
                if (event.type == Event::Closed) { window.close(); return 2; }
                if (event.type == Event::KeyPressed)
                {
                    if (event.key.code == Keyboard::Up)
                        selected = (selected + 2) % 3;
                    if (event.key.code == Keyboard::Down)
                        selected = (selected + 1) % 3;
                    if (event.key.code == Keyboard::Enter)
                        return selected;
                }
            }

            for (int i = 0; i < 3; i++)
                opts[i]->setFillColor(i == selected ? Color::Yellow : Color::White);

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
};

#endif // GAMEOVER_H
