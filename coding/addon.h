#ifndef ADDON_H
#define ADDON_H

#include <SFML/Graphics.hpp>
using namespace sf;
using namespace std;

enum class AddOnType { PowerUp, Fire, Danger, Lives };

class AddOn
{
public:
    AddOnType type;
    Sprite    sprite;
    Texture   tex;
    float     speed  = 70.f;  // px/s, delta-time based
    bool      active = true;

    // Pulse animation
    Clock pulseClock;

    AddOn(AddOnType t, float x)
        : type(t)
    {
        const char *paths[] = {
            "img/PNG/Power-ups/powerupBlue_bolt.png",    // PowerUp
            "img/PNG/Power-ups/powerupGreen_star.png",   // Fire
            "img/PNG/Power-ups/powerupRed_shield.png",   // Danger
            "img/PNG/Power-ups/powerupYellow_bolt.png"   // Lives
        };
        if (!tex.loadFromFile(paths[static_cast<int>(t)]))
        {
            cerr << "Error loading add-on texture\n";
        }
        sprite.setTexture(tex);

        // Scale to a comfortable 50×50 px
        float desired = 50.f;
        float sc = desired / static_cast<float>(tex.getSize().x);
        sprite.setScale(sc, sc);
        sprite.setOrigin(tex.getSize().x / 2.f, tex.getSize().y / 2.f);
        sprite.setPosition(x, -60.f);
    }

    void update(float dt)
    {
        sprite.move(0.f, speed * dt);

        // Gentle pulse scale
        float t     = pulseClock.getElapsedTime().asSeconds();
        float pulse = 1.f + 0.06f * std::sin(t * 4.f);
        float desired = 50.f;
        float sc    = desired / static_cast<float>(tex.getSize().x) * pulse;
        sprite.setScale(sc, sc);
    }

    void draw(RenderWindow &window)
    {
        if (!active) return;
        window.draw(sprite);
    }

    FloatRect getGlobalBounds() const { return sprite.getGlobalBounds(); }

    bool isOffScreen(float windowHeight) const
    {
        return sprite.getPosition().y > windowHeight + 80.f;
    }
};

#endif // ADDON_H
