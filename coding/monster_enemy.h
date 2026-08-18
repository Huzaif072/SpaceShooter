#ifndef MONSTER_ENEMY_H
#define MONSTER_ENEMY_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
using namespace sf;
using namespace std;

// A brief lightning bolt drawn from monster to bottom of screen
struct LightningBolt
{
    RectangleShape beam;
    Clock          timer;
    bool           active = true;

    LightningBolt(float boltX, float fromY, float screenHeight)
    {
        float h = screenHeight - fromY;
        beam.setSize(Vector2f(10.f, h));
        beam.setPosition(boltX - 5.f, fromY);
        beam.setFillColor(Color(180, 180, 255, 200));
        beam.setOutlineColor(Color::White);
        beam.setOutlineThickness(1);
    }

    void update()
    {
        // Flicker
        float t     = timer.getElapsedTime().asSeconds();
        Uint8 alpha = static_cast<Uint8>(180 + 75 * std::sin(t * 40.f));
        beam.setFillColor(Color(180, 180, 255, alpha));
        if (t > 0.35f) active = false;
    }

    void draw(RenderWindow &window) { window.draw(beam); }
};

class MonsterEnemy
{
public:
    Sprite  sprite;
    Texture texture;
    float   speed       = 90.0f;
    int     direction   = 1;
    float   lifetime    = 10.0f;  // monster stays 10 s
    float   fireInterval = 2.0f;
    bool    active      = false;
    bool    scored      = false;

    vector<LightningBolt> bolts;
    Clock lifetimeClock;
    Clock fireClock;

    MonsterEnemy()
    {
        if (!texture.loadFromFile("img/monster.png"))
            cerr << "Error loading monster texture" << endl;
        sprite.setTexture(texture);
        // Scale to 200 px wide
        float desired = 200.f;
        float sc = desired / static_cast<float>(texture.getSize().x);
        sprite.setScale(sc, sc);
        sprite.setOrigin(texture.getSize().x / 2.f, texture.getSize().y / 2.f);
        // Hide off-screen until spawn() is called
        sprite.setPosition(-9999.f, -9999.f);
    }

    void spawn(float windowWidth)
    {
        bolts.clear();
        lifetimeClock.restart();
        fireClock.restart();
        active = true;
        scored = false;
        // Centre horizontally, near top
        sprite.setPosition(windowWidth / 2.f, 80.f);
    }

    void update(float dt, float windowWidth, float spaceshipCX, float screenHeight)
    {
        if (!active) return;

        if (lifetimeClock.getElapsedTime().asSeconds() > lifetime)
        {
            active = false;
            return;
        }

        // Move left-right
        float w    = sprite.getGlobalBounds().width;
        float newX = sprite.getPosition().x + speed * direction * dt;
        if (newX <= 0.f || newX + w >= windowWidth) direction *= -1;
        sprite.move(speed * direction * dt, 0.f);

        // Fire lightning
        if (fireClock.getElapsedTime().asSeconds() >= fireInterval)
        {
            float fromY = sprite.getPosition().y + sprite.getGlobalBounds().height;
            bolts.emplace_back(spaceshipCX, fromY, screenHeight);
            fireClock.restart();
        }

        for (auto &b : bolts) b.update();
        bolts.erase(
            remove_if(bolts.begin(), bolts.end(), [](const LightningBolt &b) { return !b.active; }),
            bolts.end());
    }

    // Returns true if any active bolt overlaps spaceship
    bool checkLightningHit(FloatRect spaceshipBounds) const
    {
        for (const auto &b : bolts)
            if (b.active && b.beam.getGlobalBounds().intersects(spaceshipBounds))
                return true;
        return false;
    }

    void draw(RenderWindow &window)
    {
        if (!active) return;
        window.draw(sprite);
        for (auto &b : bolts) b.draw(window);
    }
};

#endif // MONSTER_ENEMY_H
