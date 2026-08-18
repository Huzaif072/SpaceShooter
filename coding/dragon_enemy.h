#ifndef DRAGON_ENEMY_H
#define DRAGON_ENEMY_H

#include <SFML/Graphics.hpp>
#include <vector>
using namespace sf;
using namespace std;

struct DragonFire
{
    Sprite sprite;
    float  dx, dy;   // px/s

    DragonFire(Texture &tex, float x, float y, float vx, float vy) : dx(vx), dy(vy)
    {
        sprite.setTexture(tex);
        // Scale laser to 10 px wide, 2.5x tall
        float desired = 10.f;
        float sc = desired / static_cast<float>(tex.getSize().x);
        sprite.setScale(sc, sc * 2.5f);
        sprite.setOrigin(tex.getSize().x / 2.f, 0.f);
        sprite.setPosition(x, y);
        sprite.setColor(Color(255, 80, 0));
    }

    void update(float dt) { sprite.move(dx * dt, dy * dt); }
    FloatRect getGlobalBounds() const { return sprite.getGlobalBounds(); }
};

class DragonEnemy
{
public:
    Sprite  sprite;
    Texture texture;
    Texture fireTexture;
    float   lifetime     = 5.0f;   // dragon stays 5 s per spec
    float   fireInterval = 0.35f;
    bool    active  = false;
    bool    scored  = false;

    vector<DragonFire> fires;
    Clock lifetimeClock;
    Clock fireClock;

    DragonEnemy()
    {
        if (!texture.loadFromFile("img/dragon2.png"))
            cerr << "Error loading dragon texture" << endl;
        sprite.setTexture(texture);
        // Scale to 220 px wide
        float desired = 220.f;
        float sc = desired / static_cast<float>(texture.getSize().x);
        sprite.setScale(sc, sc);
        sprite.setOrigin(texture.getSize().x / 2.f, texture.getSize().y / 2.f);
        // Hide off-screen until spawn() is called
        sprite.setPosition(-9999.f, -9999.f);

        if (!fireTexture.loadFromFile("img/PNG/Lasers/laserRed01.png"))
            cerr << "Error loading dragon fire texture" << endl;
    }

    void spawn(float windowWidth)
    {
        fires.clear();
        lifetimeClock.restart();
        fireClock.restart();
        active = true;
        scored = false;
        // Centre horizontally, near top
        sprite.setPosition(windowWidth / 2.f, 80.f);
    }

    void update(float dt, float spaceshipX, float windowWidth, float windowHeight)
    {
        if (!active) return;

        if (lifetimeClock.getElapsedTime().asSeconds() > lifetime)
        {
            active = false;
            return;
        }

        if (fireClock.getElapsedTime().asSeconds() >= fireInterval)
        {
            float cx  = sprite.getPosition().x + sprite.getGlobalBounds().width  / 2.f;
            float cy  = sprite.getPosition().y + sprite.getGlobalBounds().height;
            const float spd = 300.f;  // px/s

            float third = windowWidth / 3.f;

            // Always fire straight down
            fires.emplace_back(fireTexture, cx, cy, 0.f, spd);

            if (spaceshipX < third)
            {
                // Left zone: add left-down stream
                fires.emplace_back(fireTexture, cx, cy, -spd * 0.707f, spd * 0.707f);
            }
            else if (spaceshipX > 2.f * third)
            {
                // Right zone: add right-down stream
                fires.emplace_back(fireTexture, cx, cy,  spd * 0.707f, spd * 0.707f);
            }
            else
            {
                // Centre zone: add both diagonals
                fires.emplace_back(fireTexture, cx, cy, -spd * 0.5f,  spd * 0.866f);
                fires.emplace_back(fireTexture, cx, cy,  spd * 0.5f,  spd * 0.866f);
            }

            fireClock.restart();
        }

        for (auto &f : fires) f.update(dt);
        fires.erase(
            remove_if(fires.begin(), fires.end(), [&](const DragonFire &f) {
                auto p = f.sprite.getPosition();
                return p.y > windowHeight || p.x < -50 || p.x > windowWidth + 50;
            }),
            fires.end());
    }

    bool checkFireHit(FloatRect spaceshipBounds) const
    {
        for (const auto &f : fires)
            if (f.sprite.getGlobalBounds().intersects(spaceshipBounds))
                return true;
        return false;
    }

    void draw(RenderWindow &window)
    {
        if (!active) return;
        window.draw(sprite);
        for (auto &f : fires) window.draw(f.sprite);
    }
};

#endif // DRAGON_ENEMY_H
