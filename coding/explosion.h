#ifndef EXPLOSION_H
#define EXPLOSION_H

#include <SFML/Graphics.hpp>
using namespace sf;

// Single explosion instance — fades out over 0.5 s
class Explosion
{
public:
    Sprite sprite;
    Clock  clock;
    float  duration = 0.45f;
    bool   done     = false;

    Explosion(Texture &tex, float x, float y)
    {
        sprite.setTexture(tex);
        float desired = 80.f;
        float sc = desired / tex.getSize().x;
        sprite.setScale(sc, sc);
        sprite.setOrigin(tex.getSize().x / 2.f, tex.getSize().y / 2.f);
        sprite.setPosition(x, y);
    }

    void update()
    {
        float t = clock.getElapsedTime().asSeconds();
        if (t >= duration) { done = true; return; }

        float life  = 1.f - t / duration;
        float scale = (80.f + 40.f * (1.f - life)) / sprite.getTexture()->getSize().x;
        sprite.setScale(scale, scale);
        sprite.setColor(Color(255, 255, 255, static_cast<Uint8>(life * 255)));
    }

    void draw(RenderWindow &window) { if (!done) window.draw(sprite); }
};

#endif // EXPLOSION_H
