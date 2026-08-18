#ifndef BULLET_H
#define BULLET_H

#include <SFML/Graphics.hpp>
using namespace sf;

class Bullet
{
public:
    Sprite sprite;
    float dx = 0.0f;
    float dy = -400.f;   // px/s upward (delta-time based)
    bool isPiercing = false;

    Bullet(Texture &texture, float startX, float startY,
           float vx = 0.0f, float vy = -400.f, bool piercing = false)
        : dx(vx), dy(vy), isPiercing(piercing)
    {
        sprite.setTexture(texture);
        float desired = 10.f;
        float texW    = static_cast<float>(texture.getSize().x);
        float sc      = desired / texW;
        sprite.setScale(sc, sc * 2.5f);
        sprite.setOrigin(texture.getSize().x / 2.f, texture.getSize().y / 2.f);
        sprite.setPosition(startX, startY);
    }

    void update(float dt)
    {
        sprite.move(dx * dt, dy * dt);
    }

    FloatRect getGlobalBounds() const { return sprite.getGlobalBounds(); }

    void setPosition(float x, float y) { sprite.setPosition(x, y); }

    bool isOffScreen(float winW, float winH) const
    {
        auto pos = sprite.getPosition();
        return pos.x < -100 || pos.x > winW + 100 ||
               pos.y < -100 || pos.y > winH + 100;
    }
};

#endif // BULLET_H
