#ifndef METEOR_H
#define METEOR_H

#include <SFML/Graphics.hpp>
#include <cstdlib>
using namespace sf;

// Decorative background meteors drifting down
class Meteor
{
public:
    Sprite sprite;
    float  speedY;
    float  speedX;
    float  rotSpeed;

    Meteor(Texture &tex, float x, float y, float vy, float vx, float rot)
        : speedY(vy), speedX(vx), rotSpeed(rot)
    {
        sprite.setTexture(tex);
        float desired = 30.f + static_cast<float>(rand() % 50);
        float sc = desired / tex.getSize().x;
        sprite.setScale(sc, sc);
        sprite.setOrigin(tex.getSize().x / 2.f, tex.getSize().y / 2.f);
        sprite.setPosition(x, y);
        sprite.setColor(Color(255, 255, 255, 160));
    }

    void update(float dt, float W, float H)
    {
        sprite.move(speedX * dt, speedY * dt);
        sprite.rotate(rotSpeed * dt);
        auto p = sprite.getPosition();
        if (p.y > H + 100) sprite.setPosition(static_cast<float>(rand() % static_cast<int>(W)), -100.f);
        if (p.x < -100)    sprite.setPosition(W + 100.f, p.y);
        if (p.x > W + 100) sprite.setPosition(-100.f, p.y);
    }

    void draw(RenderWindow &w) { w.draw(sprite); }
};

#endif // METEOR_H
