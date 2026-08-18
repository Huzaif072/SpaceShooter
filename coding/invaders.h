#ifndef INVADERS_H
#define INVADERS_H

#include "enemy.h"
#include "bomb.h"
#include <ctime>
#include <cstdlib>

// ── Enemy sprite sets per type ──────────────────────────────────────────────
// Alpha = Red family,  Beta = Blue family,  Gamma = Green family
// Shape variant is chosen per instance (1-4) for variety within a wave.

class Invader : public Enemy
{
public:
    float minInterval;
    float maxInterval;
    float dropInterval;
    float timeSinceLastDrop;
    Texture &bombTexture;
    std::vector<Bomb> bombs;
    float moveSpeed     = 60.f;   // px/s
    int   moveDirection = 1;
    float screenWidth   = 1920.f;
    float bombSpeed     = 160.f;  // px/s

    Invader(const std::string &texturePath, int scoreValue,
            float minInterval, float maxInterval, Texture &bombTexture)
        : Enemy(texturePath, scoreValue),
          minInterval(minInterval), maxInterval(maxInterval),
          bombTexture(bombTexture)
    {
        dropInterval       = randomInterval();
        timeSinceLastDrop  = static_cast<float>(rand() % 10000) / 10000.f * dropInterval;

        // Auto-scale to 60 px wide
        float desired = 60.f;
        float texW    = static_cast<float>(texture.getSize().x);
        float sc      = desired / texW;
        sprite.setScale(sc, sc);
        sprite.setOrigin(texture.getSize().x / 2.f, texture.getSize().y / 2.f);
    }

    float randomInterval() const
    {
        float range = maxInterval - minInterval;
        return minInterval + static_cast<float>(rand() % 10000) / 10000.f * range;
    }

    void update(float dt) override
    {
        // Always update in-flight bombs, even after the enemy dies,
        // so they keep flying instead of vanishing with the enemy.
        for (auto &b : bombs) b.update(dt);
        bombs.erase(
            std::remove_if(bombs.begin(), bombs.end(),
                [](const Bomb &b) { return b.sprite.getPosition().y > 3000.f; }),
            bombs.end());

        if (!alive) return;   // stop firing/moving but bombs keep going

        timeSinceLastDrop += dt;
        if (timeSinceLastDrop >= dropInterval)
        {
            dropBomb();
            dropInterval       = randomInterval();   // fresh random interval
            timeSinceLastDrop  = 0.f;
        }

        sprite.move(moveSpeed * moveDirection * dt, 0.f);

        float left  = sprite.getPosition().x - sprite.getGlobalBounds().width  / 2.f;
        float right = sprite.getPosition().x + sprite.getGlobalBounds().width  / 2.f;
        if (left <= 0.f || right >= screenWidth) moveDirection *= -1;
    }

    void dropBomb() override
    {
        float cx = sprite.getPosition().x;
        float cy = sprite.getPosition().y + sprite.getGlobalBounds().height / 2.f;
        Bomb b(bombTexture, cx, cy);
        b.speed = bombSpeed;
        bombs.push_back(b);
    }

    std::vector<Bomb> &getBombs() { return bombs; }
};

// ── Helper: pick a random shape variant (1-4) ────────────────────────────────
static inline std::string pickEnemy(const char *color, int variant)
{
    return std::string("img/PNG/Enemies/enemy") + color +
           std::to_string(variant) + ".png";
}

// Alpha = Red  (4-7 s), Beta = Blue (2.5-4 s), Gamma = Green (1.5-3 s)

class AlphaInvader : public Invader
{
public:
    AlphaInvader(Texture &bombTexture)
        : Invader(pickEnemy("Red",   1 + rand() % 4), 10, 4.0f, 7.0f, bombTexture) {}
};

class BetaInvader : public Invader
{
public:
    BetaInvader(Texture &bombTexture)
        : Invader(pickEnemy("Blue",  1 + rand() % 4), 20, 2.5f, 4.0f, bombTexture) {}
};

class GammaInvader : public Invader
{
public:
    GammaInvader(Texture &bombTexture)
        : Invader(pickEnemy("Green", 1 + rand() % 4), 30, 1.5f, 3.0f, bombTexture) {}
};

#endif // INVADERS_H
