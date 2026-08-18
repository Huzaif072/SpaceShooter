#ifndef BOMB_H
#define BOMB_H

#include <SFML/Graphics.hpp>

using namespace sf;
using namespace std;

class Bomb
{
public:
    Sprite sprite;
    float speed = 160.f; // pixels per SECOND (delta-time based)

    Bomb(Texture &texture, float startX, float startY)
    {
        sprite.setTexture(texture);
        sprite.setOrigin(texture.getSize().x / 2.f, 0.f);
        sprite.setPosition(startX, startY);
        // Scale so the pink diamond is ~18x18 px on screen
        float desired = 18.f;
        float texW    = static_cast<float>(texture.getSize().x);
        float sc      = desired / texW;
        sprite.setScale(sc, sc);
    }

    // Pass delta time so speed is frame-rate independent
    void update(float dt)
    {
        sprite.move(0.f, speed * dt);
    }

    FloatRect getGlobalBounds() const { return sprite.getGlobalBounds(); }
    void setPosition(float x, float y) { sprite.setPosition(x, y); }
};

#endif // BOMB_H
