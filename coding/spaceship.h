#ifndef SPACESHIP_H
#define SPACESHIP_H

#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>
#include "bullet.h"

using namespace sf;
using namespace std;

class Spaceship
{
public:
    Texture tex;
    Sprite  sprite;

    // Shield overlay (shown when powerUpMode is active)
    Texture shieldTex;
    Sprite  shieldSprite;

    float speed       = 220.f;   // px/s
    vector<Bullet> bullets;
    Texture bulletTexture;       // set externally by Game (blue laser)
    Texture fireBulletTexture;   // green laser for fire mode

    float fireCooldown = 0.35f;
    Clock fireClock;

    bool powerUpMode = false;
    bool fireMode    = false;

    Spaceship(const string &png_path, Texture &blueLaser, Texture &greenLaser)
        : bulletTexture(blueLaser), fireBulletTexture(greenLaser)
    {
        if (!tex.loadFromFile(png_path))
            cerr << "Error loading spaceship: " << png_path << "\n";
        sprite.setTexture(tex);
        float desired = 80.f;
        float sc      = desired / tex.getSize().x;
        sprite.setScale(sc, sc);
        sprite.setOrigin(tex.getSize().x / 2.f, tex.getSize().y / 2.f);

        if (!shieldTex.loadFromFile("img/PNG/Effects/shield3.png"))
            cerr << "Error loading shield texture\n";
        shieldSprite.setTexture(shieldTex);
        float shSc = 100.f / shieldTex.getSize().x;
        shieldSprite.setScale(shSc, shSc);
        shieldSprite.setOrigin(shieldTex.getSize().x / 2.f, shieldTex.getSize().y / 2.f);
        shieldSprite.setColor(Color(80, 200, 255, 160));
    }

    void fire()
    {
        if (fireClock.getElapsedTime().asSeconds() < fireCooldown) return;

        float bx  = sprite.getPosition().x;
        float by  = sprite.getPosition().y - sprite.getGlobalBounds().height / 2.f;
        const float spd = 400.f;

        Texture &tex = fireMode ? fireBulletTexture : bulletTexture;

        if (powerUpMode)
        {
            float dirs[7][2] = {
                {0,          -spd},
                {-spd*.5f,  -spd*.866f},
                { spd*.5f,  -spd*.866f},
                {-spd*.707f,-spd*.707f},
                { spd*.707f,-spd*.707f},
                {-spd,       0},
                { spd,       0}
            };
            for (auto &d : dirs)
                bullets.emplace_back(tex, bx, by, d[0], d[1], fireMode);
        }
        else
        {
            bullets.emplace_back(tex, bx, by, 0.f, -spd, fireMode);
        }
        fireClock.restart();
    }

    void move(bool left, bool right, bool up, bool down, const RenderWindow &window, float dt)
    {
        float dx = 0.f, dy = 0.f;
        if (left)  dx -= speed;
        if (right) dx += speed;
        if (up)    dy -= speed;
        if (down)  dy += speed;

        if (dx != 0.f && dy != 0.f) { dx *= 0.7071f; dy *= 0.7071f; }

        float W = static_cast<float>(window.getSize().x);
        float H = static_cast<float>(window.getSize().y);
        float hw = sprite.getGlobalBounds().width  / 2.f;
        float hh = sprite.getGlobalBounds().height / 2.f;

        float nx = sprite.getPosition().x + dx * dt;
        float ny = sprite.getPosition().y + dy * dt;

        // Horizontal wrap-around
        if (nx + hw < 0.f) nx = W + hw;
        if (nx - hw > W)   nx = -hw;

        // Vertical clamp
        ny = max(hh, min(H - hh, ny));

        sprite.setPosition(nx, ny);
        shieldSprite.setPosition(nx, ny);
    }
};

#endif // SPACESHIP_H
