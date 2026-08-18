#ifndef ENEMY_H
#define ENEMY_H

#include <SFML/Graphics.hpp>
#include <string>

using namespace sf;

class Enemy
{
public:
    Sprite sprite;
    Texture texture;
    int scoreValue;
    bool alive = true; // set false when killed

    Enemy(const std::string &texturePath, int scoreValue) : scoreValue(scoreValue)
    {
        if (!texture.loadFromFile(texturePath))
            throw std::runtime_error("Error loading texture: " + texturePath);
        sprite.setTexture(texture);
    }

    virtual ~Enemy() = default;
    virtual void dropBomb() = 0;
    virtual void update(float deltaTime) = 0;

    int getScoreValue() const { return scoreValue; }
};

// ── Monster and Dragon stubs (not used via Enemy pointer) ──────────────────
// kept for reference; full implementations are in monster_enemy.h / dragon_enemy.h

#endif // ENEMY_H
