#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <memory>
#include <fstream>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <string>
#include <vector>

#include "start_screen.h"
#include "name_entry_screen.h"
#include "menu.h"
#include "spaceship.h"
#include "bullet.h"
#include "pausemenu.h"
#include "enemy.h"
#include "invaders.h"
#include "bomb.h"
#include "addon.h"
#include "monster_enemy.h"
#include "dragon_enemy.h"
#include "gameover.h"
#include "endscreen.h"
#include "explosion.h"
#include "meteor.h"

using namespace std;
using namespace sf;

const char title[] = "Spaceshooter";

// ─────────────────────────────────────────────────────────────────────────────
class Game
{
public:
    Sprite  background;
    Texture bg_texture;
    Spaceship *p = nullptr;
    string playerName;

    vector<unique_ptr<Enemy>> enemies;

    // Shared textures for bullets & bombs
    Texture blueLaserTex;
    Texture greenLaserTex;
    Texture redLaserTex;
    Texture bombTexture;     // red laser for enemy bombs

    Game();
    void start();

private:
    // ── Core loop ──────────────────────────────────────────────────────────
    void loadResources();
    void resetGameState();
    void runGameLoop();
    void handlePauseMenu();

    // ── Per-frame helpers ──────────────────────────────────────────────────
    void updateTimers(float dt);
    void updateEnemies(float dt);
    void updateAddOns(float dt);
    void handleCollisions();
    void applyAddOn(AddOnType t);
    void checkPhaseCompletion();
    void spawnPhaseEnemies();
    void spawnSpecialEnemy();
    void spawnAddOnIfNeeded();
    void spawnExplosion(float x, float y);
    void render(float dt);
    void updateHUD();
    void showLevelAnnouncement(int lvl);
    void initMeteors();

    // ── Utility ────────────────────────────────────────────────────────────
    string getPlayerBadge() const;
    void saveScore() const;

    // ── Window ─────────────────────────────────────────────────────────────
    RenderWindow window;

    // ── HUD ────────────────────────────────────────────────────────────────
    Font font;
    Text scoreText, livesText, timerText, levelText, phaseText;
    Text powerUpText, fireModeText, badgeText;

    // Life icon for HUD
    Texture lifeIconTex;

    // ── Game state ──────────────────────────────────────────────────────────
    int  score  = 0;
    int  lives  = 3;
    int  level  = 1;
    int  phase  = 1;
    bool isPaused = false;
    Clock gameClock;
    Time  totalTime;

    // ── Special enemies ─────────────────────────────────────────────────────
    MonsterEnemy monster;
    DragonEnemy  dragon;
    bool monsterActive = false;
    bool dragonActive  = false;
    bool specialPhase  = false;

    // ── Add-ons ─────────────────────────────────────────────────────────────
    vector<AddOn> addons;
    Clock  addonClock;
    float  addonInterval = 14.f;

    // ── Power-up / fire-mode timers ─────────────────────────────────────────
    bool  powerUpActive  = false;
    bool  fireModeActive = false;
    Clock powerUpClock;
    Clock fireModeClock;

    // ── Screen flash on hit ─────────────────────────────────────────────────
    bool  hitFlash    = false;
    Clock hitClock;

    // ── Explosions ──────────────────────────────────────────────────────────
    Texture explosionTex;
    vector<Explosion> explosions;

    // ── Meteors (background decoration) ────────────────────────────────────
    Texture meteorTex;
    vector<Meteor> meteors;

    // ── Pause menu ─────────────────────────────────────────────────────────
    PauseMenu pauseMenu;
};

// ═════════════════════════════════════════════════════════════════════════════
//  Constructor
// ═════════════════════════════════════════════════════════════════════════════
Game::Game()
    : window(VideoMode::getDesktopMode(), title, Style::Fullscreen),
      pauseMenu(window, background)
{
    srand(static_cast<unsigned>(time(nullptr)));
    loadResources();
}

// ─────────────────────────────────────────────────────────────────────────────
void Game::loadResources()
{
    float sw = static_cast<float>(window.getSize().x);
    float sh = static_cast<float>(window.getSize().y);

    // ── Background (Milky Way) ────────────────────────────────────────────
    if (!bg_texture.loadFromFile("img/background1.jpg"))
        cerr << "Error loading background\n";
    background.setTexture(bg_texture);
    background.setScale(sw / bg_texture.getSize().x, sh / bg_texture.getSize().y);

    // ── Laser textures ────────────────────────────────────────────────────
    if (!blueLaserTex.loadFromFile("img/PNG/Lasers/laserBlue01.png"))
        cerr << "Error loading blue laser\n";
    if (!greenLaserTex.loadFromFile("img/PNG/Lasers/laserGreen01.png"))
        cerr << "Error loading green laser\n";
    if (!redLaserTex.loadFromFile("img/PNG/Lasers/laserRed01.png"))
        cerr << "Error loading red laser\n";

    // Enemy bombs use the red laser
    bombTexture = redLaserTex;

    // ── Player ship ───────────────────────────────────────────────────────
    p = new Spaceship("img/Spaceship2.png", blueLaserTex, greenLaserTex);
    p->sprite.setPosition(sw / 2.f, sh - 80.f);
    p->shieldSprite.setPosition(sw / 2.f, sh - 80.f);

    // ── Life icon ─────────────────────────────────────────────────────────
    if (!lifeIconTex.loadFromFile("img/PNG/UI/playerLife2_orange.png"))
        cerr << "Error loading life icon\n";

    // ── Explosion ─────────────────────────────────────────────────────────
    if (!explosionTex.loadFromFile("img/explosion.png"))
        cerr << "Error loading explosion\n";

    // ── Meteors ───────────────────────────────────────────────────────────
    if (!meteorTex.loadFromFile("img/PNG/Meteors/meteorBrown_big1.png"))
        cerr << "Error loading meteor\n";

    // ── Font ──────────────────────────────────────────────────────────────
    if (!font.loadFromFile("Fonts/ariblk.ttf"))
        cerr << "Error loading font\n";

    auto setupText = [&](Text &t, unsigned sz) {
        t.setFont(font);
        t.setCharacterSize(sz);
        t.setFillColor(Color::White);
    };
    setupText(scoreText,    20);
    setupText(livesText,    20);
    setupText(timerText,    18);
    setupText(levelText,    20);
    setupText(phaseText,    18);
    setupText(powerUpText,  18);
    setupText(fireModeText, 18);
    setupText(badgeText,    18);
    powerUpText.setFillColor(Color::Yellow);
    fireModeText.setFillColor(Color(255, 130, 0));
    badgeText.setFillColor(Color(255, 215, 0));

    initMeteors();
}

// ─────────────────────────────────────────────────────────────────────────────
void Game::initMeteors()
{
    float W = static_cast<float>(window.getSize().x);
    float H = static_cast<float>(window.getSize().y);
    meteors.clear();
    for (int i = 0; i < 18; i++)
    {
        float x    = static_cast<float>(rand() % static_cast<int>(W));
        float y    = static_cast<float>(rand() % static_cast<int>(H));
        float vy   = 40.f + static_cast<float>(rand() % 80);
        float vx   = -15.f + static_cast<float>(rand() % 30);
        float rot  = -30.f + static_cast<float>(rand() % 60);
        meteors.emplace_back(meteorTex, x, y, vy, vx, rot);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
void Game::resetGameState()
{
    score = 0; lives = 3; level = 1; phase = 1;
    isPaused      = false;
    powerUpActive = false; fireModeActive = false;
    monsterActive = false; dragonActive   = false;
    specialPhase  = false; hitFlash       = false;

    p->powerUpMode  = false;
    p->fireMode     = false;
    p->fireCooldown = 0.35f;
    p->bullets.clear();

    float sw = static_cast<float>(window.getSize().x);
    float sh = static_cast<float>(window.getSize().y);
    p->sprite.setPosition(sw / 2.f, sh - 80.f);
    p->shieldSprite.setPosition(sw / 2.f, sh - 80.f);

    enemies.clear();
    addons.clear();
    explosions.clear();
    gameClock.restart();
    totalTime    = Time::Zero;
    addonClock.restart();
    addonInterval = 14.f;

    showLevelAnnouncement(level);
    spawnPhaseEnemies();
}

// ═════════════════════════════════════════════════════════════════════════════
//  Entry point
// ═════════════════════════════════════════════════════════════════════════════
void Game::start()
{
    StartScreen startScreen(window);
    startScreen.show();

    if (startScreen.isFinished())
    {
        NameEntryScreen nes(window, bg_texture, background);
        nes.show();
        playerName = nes.getPlayerName();
    }

    while (window.isOpen())
    {
        Menu menu(window, playerName);
        bool play = false;
        while (window.isOpen())
            if (menu.run()) { play = true; break; }
        if (!play || !window.isOpen()) break;

        resetGameState();
        runGameLoop();
    }
}

// ═════════════════════════════════════════════════════════════════════════════
//  Main game loop
// ═════════════════════════════════════════════════════════════════════════════
void Game::runGameLoop()
{
    Clock frameClock;
    bool kLeft = false, kRight = false, kUp = false, kDown = false, kSpace = false;

    while (window.isOpen())
    {
        float dt = frameClock.restart().asSeconds();
        if (dt > 0.05f) dt = 0.05f;

        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed) { window.close(); return; }
            if (event.type == Event::KeyPressed)
            {
                switch (event.key.code)
                {
                case Keyboard::Left:  kLeft  = true; break;
                case Keyboard::Right: kRight = true; break;
                case Keyboard::Up:    kUp    = true; break;
                case Keyboard::Down:  kDown  = true; break;
                case Keyboard::Space: kSpace = true; break;
                case Keyboard::Escape:
                    isPaused = !isPaused;
                    if (isPaused) { totalTime += gameClock.getElapsedTime();
                                    kLeft = kRight = kUp = kDown = kSpace = false; }
                    else            gameClock.restart();
                    break;
                default: break;
                }
            }
            if (event.type == Event::KeyReleased)
            {
                switch (event.key.code)
                {
                case Keyboard::Left:  kLeft  = false; break;
                case Keyboard::Right: kRight = false; break;
                case Keyboard::Up:    kUp    = false; break;
                case Keyboard::Down:  kDown  = false; break;
                case Keyboard::Space: kSpace = false; break;
                default: break;
                }
            }
        }

        if (isPaused) { handlePauseMenu(); continue; }

        // ── Player ──────────────────────────────────────────────────────
        p->move(kLeft, kRight, kUp, kDown, window, dt);
        if (kSpace) p->fire();

        // ── Updates ─────────────────────────────────────────────────────
        updateTimers(dt);
        updateEnemies(dt);
        updateAddOns(dt);

        // Bullets (delta-time based)
        float winW = static_cast<float>(window.getSize().x);
        float winH = static_cast<float>(window.getSize().y);
        for (auto &b : p->bullets) b.update(dt);
        p->bullets.erase(
            remove_if(p->bullets.begin(), p->bullets.end(),
                [&](const Bullet &b) { return b.isOffScreen(winW, winH); }),
            p->bullets.end());

        // Explosions
        for (auto &e : explosions) e.update();
        explosions.erase(
            remove_if(explosions.begin(), explosions.end(),
                [](const Explosion &e) { return e.done; }),
            explosions.end());

        // Meteors
        for (auto &m : meteors) m.update(dt, winW, winH);

        handleCollisions();

        // Remove dead enemies — but ONLY after their in-flight bombs have cleared.
        // This prevents bombs from vanishing when their owner is killed.
        enemies.erase(
            remove_if(enemies.begin(), enemies.end(),
                [](const unique_ptr<Enemy> &e) {
                    if (e->alive) return false;
                    // Keep dead invaders while they still have bombs in flight
                    if (auto *inv = dynamic_cast<Invader *>(e.get()))
                        return inv->getBombs().empty();
                    return true;   // non-invader specials: erase immediately
                }),
            enemies.end());

        // Remove collected/offscreen add-ons
        addons.erase(
            remove_if(addons.begin(), addons.end(), [&](AddOn &a) {
                if (a.isOffScreen(winH))
                {
                    if (a.type == AddOnType::Danger) score += 5;
                    return true;
                }
                return !a.active;
            }),
            addons.end());

        checkPhaseCompletion();
        spawnAddOnIfNeeded();
        updateHUD();
        render(dt);

        if (lives <= 0)
        {
            saveScore();
            GameOverScreen gos(window, font, score, playerName, background);
            int choice = gos.show();
            if (choice == 0) { resetGameState(); continue; }
            return;
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
void Game::updateTimers(float /*dt*/)
{
    if (powerUpActive && powerUpClock.getElapsedTime().asSeconds() > 7.f)
    {
        powerUpActive   = false;
        p->powerUpMode  = false;
        p->fireCooldown = 0.35f;
    }
    if (fireModeActive && fireModeClock.getElapsedTime().asSeconds() > 7.f)
    {
        fireModeActive = false;
        p->fireMode    = false;
    }
}

void Game::updateEnemies(float dt)
{
    if (!specialPhase)
        for (auto &e : enemies) e->update(dt);

    float sw = static_cast<float>(window.getSize().x);
    float sh = static_cast<float>(window.getSize().y);
    float spCX = p->sprite.getPosition().x;

    if (monsterActive) monster.update(dt, sw, spCX, sh);
    if (dragonActive)  dragon.update(dt, p->sprite.getPosition().x, sw, sh);
}

void Game::updateAddOns(float dt)
{
    for (auto &a : addons) a.update(dt);
}

// ─────────────────────────────────────────────────────────────────────────────
void Game::spawnExplosion(float x, float y)
{
    explosions.emplace_back(explosionTex, x, y);
}

// ─────────────────────────────────────────────────────────────────────────────
void Game::handleCollisions()
{
    FloatRect spBounds = p->sprite.getGlobalBounds();
    bool invincible    = powerUpActive;

    // ── 1. Player bullets vs enemies ─────────────────────────────────────
    for (auto &bullet : p->bullets)
    {
        for (auto &enemy : enemies)
        {
            if (!enemy->alive) continue;
            if (bullet.sprite.getGlobalBounds().intersects(enemy->sprite.getGlobalBounds()))
            {
                score += enemy->scoreValue;
                spawnExplosion(enemy->sprite.getPosition().x,
                               enemy->sprite.getPosition().y);
                enemy->alive = false;
                if (!bullet.isPiercing)
                {
                    bullet.setPosition(-9999.f, -9999.f);
                    break;
                }
            }
        }
    }

    if (invincible) return;

    // ── 2. Enemy bombs vs player ──────────────────────────────────────────
    for (auto &enemy : enemies)
    {
        auto *inv = dynamic_cast<Invader *>(enemy.get());
        if (!inv) continue;
        for (auto &bomb : inv->getBombs())
        {
            if (bomb.sprite.getGlobalBounds().intersects(spBounds))
            {
                lives--;
                bomb.setPosition(-9999.f, -9999.f);
                hitFlash = true; hitClock.restart();
            }
        }
    }

    // ── 3. Enemy sprite vs player ─────────────────────────────────────────
    for (auto &enemy : enemies)
    {
        if (!enemy->alive) continue;
        if (enemy->sprite.getGlobalBounds().intersects(spBounds))
        {
            lives--;
            spawnExplosion(enemy->sprite.getPosition().x,
                           enemy->sprite.getPosition().y);
            enemy->alive = false;
            hitFlash = true; hitClock.restart();
        }
    }

    // ── 4. Monster lightning ──────────────────────────────────────────────
    if (monsterActive && monster.checkLightningHit(spBounds))
    {
        lives--;
        hitFlash = true; hitClock.restart();
    }

    // ── 5. Dragon fire ────────────────────────────────────────────────────
    if (dragonActive && dragon.checkFireHit(spBounds))
    {
        lives--;
        hitFlash = true; hitClock.restart();
    }

    // ── 6. Danger add-on ──────────────────────────────────────────────────
    for (auto &a : addons)
    {
        if (a.type == AddOnType::Danger && a.active)
        {
            if (a.getGlobalBounds().intersects(spBounds))
            {
                lives--;
                a.active = false;
                hitFlash = true; hitClock.restart();
            }
        }
    }

    // ── 7. Beneficial add-ons ─────────────────────────────────────────────
    for (auto &a : addons)
    {
        if (!a.active || a.type == AddOnType::Danger) continue;
        if (a.getGlobalBounds().intersects(spBounds))
        {
            applyAddOn(a.type);
            a.active = false;
        }
    }
}

void Game::applyAddOn(AddOnType t)
{
    switch (t)
    {
    case AddOnType::PowerUp:
        powerUpActive   = true;
        p->powerUpMode  = true;
        p->fireCooldown = 0.12f;
        powerUpClock.restart();
        break;
    case AddOnType::Fire:
        fireModeActive = true;
        p->fireMode    = true;
        fireModeClock.restart();
        break;
    case AddOnType::Lives:
        lives = min(lives + 1, 5);
        break;
    default: break;
    }
}

// ─────────────────────────────────────────────────────────────────────────────
void Game::checkPhaseCompletion()
{
    if (monsterActive && !monster.active)
    {
        monsterActive = false; specialPhase = false;
        if (!monster.scored) { score += 40; monster.scored = true; }
        spawnPhaseEnemies(); return;
    }
    if (dragonActive && !dragon.active)
    {
        dragonActive = false; specialPhase = false;
        if (!dragon.scored) { score += 50; dragon.scored = true; }
        spawnPhaseEnemies(); return;
    }
    if (specialPhase || !enemies.empty()) return;

    phase++;
    if (phase > 3)
    {
        phase = 1; level++;
        if (level > 3)
        {
            saveScore();
            EndScreen es(window, font, score, playerName, background, true);
            int choice = es.show();
            if (choice == 0) resetGameState();
            return;
        }
        EndScreen es(window, font, score, playerName, background, false);
        int choice = es.show();
        if (choice == 1) { level--; phase = 3; return; }
        showLevelAnnouncement(level);
    }

    if (rand() % 7 == 0) spawnSpecialEnemy();
    else                  spawnPhaseEnemies();
}

// ─────────────────────────────────────────────────────────────────────────────
void Game::spawnPhaseEnemies()
{
    enemies.clear();
    float speedMult = 1.f + (level - 1) * 0.5f;
    float bombMult  = 1.f + (level - 1) * 0.4f;
    int   count     = 4 + level * 2;  // 6, 8, 10

    for (int i = 0; i < count; i++)
    {
        unique_ptr<Enemy> e;
        switch (phase)
        {
        case 1: e = make_unique<AlphaInvader>(bombTexture); break;
        case 2: e = make_unique<BetaInvader> (bombTexture); break;
        case 3: e = make_unique<GammaInvader>(bombTexture); break;
        default: e = make_unique<AlphaInvader>(bombTexture); break;
        }
        if (auto *inv = dynamic_cast<Invader *>(e.get()))
        {
            inv->moveSpeed  *= speedMult;
            inv->bombSpeed   = 160.f * bombMult;
            inv->screenWidth = static_cast<float>(window.getSize().x);
            inv->scoreValue *= level;
        }
        enemies.push_back(std::move(e));
    }

    // Grid layout — 2 rows max
    float W = static_cast<float>(window.getSize().x);
    int   cols = min(count, 5);
    int   rows = (count + cols - 1) / cols;
    float spacingX = (W - 240.f) / max(cols - 1, 1);
    if (spacingX > 160.f) spacingX = 160.f;
    float startX = (W - spacingX * (cols - 1)) / 2.f;

    for (int i = 0; i < (int)enemies.size(); i++)
    {
        float x = startX + (i % cols) * spacingX;
        float y = 80.f   + (i / cols) * 120.f;
        enemies[i]->sprite.setPosition(x, y);
    }
}

void Game::spawnSpecialEnemy()
{
    specialPhase = true;
    enemies.clear();
    if (rand() % 2 == 0) { monsterActive = true; monster.spawn(static_cast<float>(window.getSize().x)); }
    else                  { dragonActive  = true; dragon.spawn (static_cast<float>(window.getSize().x)); }
}

void Game::spawnAddOnIfNeeded()
{
    if (addonClock.getElapsedTime().asSeconds() < addonInterval) return;
    addonClock.restart();
    addonInterval = 10.f + static_cast<float>(rand() % 8);

    float x = 80.f + static_cast<float>(rand() % static_cast<int>(window.getSize().x - 160));
    AddOnType t = static_cast<AddOnType>(rand() % 4);
    addons.emplace_back(t, x);
}

// ─────────────────────────────────────────────────────────────────────────────
//  HUD data update
// ─────────────────────────────────────────────────────────────────────────────
void Game::updateHUD()
{
    int secs = static_cast<int>(totalTime.asSeconds() +
                                gameClock.getElapsedTime().asSeconds());
    int m = secs / 60, s = secs % 60;
    ostringstream ts;
    ts << setw(2) << setfill('0') << m << ":" << setw(2) << setfill('0') << s;
    timerText.setString(ts.str());
    scoreText.setString(to_string(score));
}

// ─────────────────────────────────────────────────────────────────────────────
//  Render
// ─────────────────────────────────────────────────────────────────────────────
static void drawPanel(RenderWindow &w, float x, float y, float W, float H,
                      Color fill = Color(0, 5, 30, 180))
{
    RectangleShape p(Vector2f(W, H));
    p.setPosition(x, y);
    p.setFillColor(fill);
    p.setOutlineColor(Color(80, 120, 255, 80));
    p.setOutlineThickness(1.5f);
    w.draw(p);
}

void Game::render(float dt)
{
    window.clear(Color::Black);

    // ── Scrolling starfield background ──────────────────────────────────
    window.draw(background);

    // ── Meteors (behind everything) ──────────────────────────────────────
    for (auto &m : meteors) m.draw(window);

    // ── Player bullets ───────────────────────────────────────────────────
    for (auto &b : p->bullets) window.draw(b.sprite);

    // ── Enemies + their bombs ────────────────────────────────────────────
    for (auto &e : enemies)
    {
        // Always draw bombs (even for dead enemies) so they keep flying
        if (auto *inv = dynamic_cast<Invader *>(e.get()))
            for (auto &bm : inv->getBombs()) window.draw(bm.sprite);
        // Only draw the ship sprite if still alive
        if (e->alive) window.draw(e->sprite);
    }

    // ── Special enemies ──────────────────────────────────────────────────
    monster.draw(window);
    dragon.draw(window);

    // ── Add-ons ──────────────────────────────────────────────────────────
    for (auto &a : addons) a.draw(window);

    // ── Explosions ───────────────────────────────────────────────────────
    for (auto &ex : explosions) ex.draw(window);

    // ── Player ship ──────────────────────────────────────────────────────
    window.draw(p->sprite);
    if (powerUpActive)
    {
        float pulse = 0.8f + 0.2f * std::sin(gameClock.getElapsedTime().asSeconds() * 8.f);
        p->shieldSprite.setScale(
            100.f / p->shieldTex.getSize().x * pulse,
            100.f / p->shieldTex.getSize().x * pulse);
        window.draw(p->shieldSprite);
    }

    // ── Screen flash on hit ──────────────────────────────────────────────
    if (hitFlash)
    {
        float elapsed = hitClock.getElapsedTime().asSeconds();
        if (elapsed < 0.25f)
        {
            float alpha = (1.f - elapsed / 0.25f) * 120.f;
            RectangleShape flash(Vector2f(static_cast<float>(window.getSize().x),
                                          static_cast<float>(window.getSize().y)));
            flash.setFillColor(Color(255, 40, 40, static_cast<Uint8>(alpha)));
            window.draw(flash);
        }
        else hitFlash = false;
    }

    // ─────────────── HUD ─────────────────────────────────────────────────
    float W = static_cast<float>(window.getSize().x);
    float H = static_cast<float>(window.getSize().y);

    // Top-left: SCORE + TIME
    drawPanel(window, 8, 8, 280, 60);
    Text lbl; lbl.setFont(font); lbl.setCharacterSize(11);
    lbl.setFillColor(Color(160, 180, 255));
    lbl.setString("SCORE"); lbl.setPosition(14, 13); window.draw(lbl);
    lbl.setString("TIME");  lbl.setPosition(155, 13); window.draw(lbl);
    scoreText.setPosition(14, 28);  scoreText.setCharacterSize(18);
    scoreText.setFillColor(Color(255, 230, 80));
    window.draw(scoreText);
    timerText.setPosition(155, 28); timerText.setCharacterSize(18);
    timerText.setFillColor(Color::White);
    window.draw(timerText);

    // Top-centre: LEVEL + PHASE
    drawPanel(window, W/2.f - 130, 8, 260, 60);
    lbl.setString("LEVEL"); lbl.setPosition(W/2.f - 120, 13); window.draw(lbl);
    lbl.setString("PHASE"); lbl.setPosition(W/2.f + 10,  13); window.draw(lbl);
    Text lvlT; lvlT.setFont(font); lvlT.setCharacterSize(22); lvlT.setStyle(Text::Bold);
    lvlT.setFillColor(Color(80, 220, 255)); lvlT.setString(to_string(level));
    lvlT.setPosition(W/2.f - 100, 26); window.draw(lvlT);
    lvlT.setString(to_string(phase) + " / 3");
    lvlT.setCharacterSize(18); lvlT.setStyle(Text::Regular);
    lvlT.setPosition(W/2.f + 10, 28); window.draw(lvlT);

    // Top-right: LIVES using ship icons + badge
    drawPanel(window, W - 300, 8, 292, 60);
    lbl.setString("LIVES"); lbl.setPosition(W - 294, 13); window.draw(lbl);
    // Draw mini ship icons (one per life)
    Sprite lifeIcon(lifeIconTex);
    float lifeScale = 26.f / lifeIconTex.getSize().x;
    lifeIcon.setScale(lifeScale, lifeScale);
    for (int i = 0; i < 5; i++)
    {
        lifeIcon.setPosition(W - 292 + i * 32.f, 30);
        lifeIcon.setColor(i < lives ? Color::White : Color(60, 60, 80, 100));
        window.draw(lifeIcon);
    }
    // Badge top-right corner
    string badge = getPlayerBadge();
    if (!badge.empty())
    {
        Color bc = badge == "GOLD"   ? Color(255, 215, 0) :
                   badge == "SILVER" ? Color(192, 192, 192) : Color(205, 127, 50);
        badgeText.setString("[ " + badge + " ]");
        badgeText.setFillColor(bc);
        badgeText.setCharacterSize(14);
        badgeText.setPosition(W - 150, 13);
        window.draw(badgeText);
    }

    // Status pills
    if (powerUpActive)
    {
        drawPanel(window, 8, 76, 180, 26, Color(120, 100, 0, 190));
        powerUpText.setString("⚡ POWER UP!");
        powerUpText.setPosition(14, 80); powerUpText.setCharacterSize(13);
        window.draw(powerUpText);
    }
    if (fireModeActive)
    {
        drawPanel(window, 8, 108, 180, 26, Color(120, 50, 0, 190));
        fireModeText.setString("🔥 FIRE MODE!");
        fireModeText.setPosition(14, 112); fireModeText.setCharacterSize(13);
        window.draw(fireModeText);
    }

    // Player name — bottom-left
    Text nameT; nameT.setFont(font); nameT.setCharacterSize(14);
    nameT.setFillColor(Color(160, 160, 255, 200));
    nameT.setString(playerName);
    nameT.setPosition(10, H - 30);
    window.draw(nameT);

    window.display();
}

// ─────────────────────────────────────────────────────────────────────────────
//  Level announcement — particle burst + pulsing ring
// ─────────────────────────────────────────────────────────────────────────────
void Game::showLevelAnnouncement(int lvl)
{
    const float DURATION = 3.0f;
    Clock clk;

    Color lvlColor = lvl == 1 ? Color(80,  200, 255) :
                     lvl == 2 ? Color(140, 255, 90)  : Color(255, 90, 60);
    Color glowCol  = lvl == 1 ? Color(20,  80,  200) :
                     lvl == 2 ? Color(40,  160, 20)  : Color(180, 30, 10);

    const char *levelNames[] = { "", "LEVEL  1", "LEVEL  2", "LEVEL  3" };
    const char *subtitles[]  = { "", "Alpha  Squadron", "Beta  Battalion", "Gamma  Elite" };

    float W = static_cast<float>(window.getSize().x);
    float H = static_cast<float>(window.getSize().y);

    struct Particle { float x, y, vx, vy, r, life, maxLife; Color col; };
    vector<Particle> parts(120);
    for (auto &p : parts)
    {
        float angle = static_cast<float>(rand() % 6283) / 1000.f;
        float speed = 80.f + static_cast<float>(rand() % 320);
        p.x = W/2.f; p.y = H/2.f;
        p.vx = std::cos(angle) * speed;
        p.vy = std::sin(angle) * speed;
        p.r  = 2.f + static_cast<float>(rand() % 5);
        p.maxLife = 0.6f + static_cast<float>(rand() % 14) / 10.f;
        p.life = 0.f;
        p.col = (rand() % 2 == 0) ? lvlColor : Color::White;
    }

    float prevT = 0.f;
    while (window.isOpen())
    {
        float t  = clk.getElapsedTime().asSeconds();
        float dt = t - prevT; prevT = t;
        if (t > DURATION) break;

        Event ev;
        while (window.pollEvent(ev)) if (ev.type == Event::Closed) { window.close(); return; }

        float alpha = t < 0.35f ? t / 0.35f :
                      t > DURATION - 0.5f ? (DURATION - t) / 0.5f : 1.f;
        alpha = max(0.f, min(1.f, alpha));
        Uint8 a8 = static_cast<Uint8>(alpha * 255);

        for (auto &p : parts)
        {
            p.life += dt; p.x += p.vx * dt; p.y += p.vy * dt;
            p.vy += 60.f * dt; p.vx *= 0.98f;
        }

        window.clear(Color(5, 5, 20));
        window.draw(background);

        RectangleShape ov(Vector2f(W, H));
        ov.setFillColor(Color(0, 0, 0, static_cast<Uint8>(alpha * 185)));
        window.draw(ov);

        // Scanline sweep
        float sweepY = H * (t / DURATION);
        RectangleShape sw(Vector2f(W, 3));
        sw.setPosition(0, sweepY);
        sw.setFillColor(Color(lvlColor.r, lvlColor.g, lvlColor.b, static_cast<Uint8>(60 * alpha)));
        window.draw(sw);

        // Pulsing rings
        for (int ri = 0; ri < 2; ri++)
        {
            float pulse = std::sin(t * 6.f + ri * 1.5f) * 0.5f + 0.5f;
            float rR = (160.f + ri * 80.f) + pulse * 35.f;
            CircleShape ring(rR);
            ring.setOrigin(rR, rR);
            ring.setPosition(W/2.f, H/2.f - 20.f);
            ring.setFillColor(Color::Transparent);
            ring.setOutlineThickness(ri == 0 ? 3.f + pulse * 3.f : 1.5f);
            ring.setOutlineColor(ri == 0 ?
                Color(lvlColor.r, lvlColor.g, lvlColor.b, static_cast<Uint8>(80 * alpha)) :
                Color(255, 255, 255, static_cast<Uint8>(25 * alpha)));
            window.draw(ring);
        }

        // Accent bars wipe from centre
        float barW = W / 2.f * min(1.f, t / 0.5f);
        for (int side : {-1, 1})
        {
            RectangleShape bar(Vector2f(barW, 5));
            bar.setPosition(side == -1 ? W/2.f - barW : W/2.f, H/2.f - 70.f);
            bar.setFillColor(Color(lvlColor.r, lvlColor.g, lvlColor.b, a8));
            window.draw(bar);
            bar.setPosition(side == -1 ? W/2.f - barW : W/2.f, H/2.f + 50.f);
            window.draw(bar);
        }

        // Particles
        for (auto &p : parts)
        {
            if (p.life > p.maxLife) continue;
            float pa = 1.f - p.life / p.maxLife;
            CircleShape dot(p.r);
            dot.setOrigin(p.r, p.r);
            dot.setPosition(p.x, p.y);
            dot.setFillColor(Color(p.col.r, p.col.g, p.col.b, static_cast<Uint8>(pa * 255)));
            window.draw(dot);
        }

        // Glow text layers
        for (int layer = 3; layer >= 0; layer--)
        {
            Text lt; lt.setFont(font);
            lt.setString(levelNames[lvl]);
            lt.setCharacterSize(96); lt.setStyle(Text::Bold);
            float off = layer * 4.f;
            Color lc = layer == 0 ?
                Color(lvlColor.r, lvlColor.g, lvlColor.b, a8) :
                Color(glowCol.r, glowCol.g, glowCol.b,
                      static_cast<Uint8>(a8 * 0.18f / layer));
            lt.setFillColor(lc);
            lt.setPosition(W/2.f - lt.getGlobalBounds().width/2.f + off, H/2.f - 80.f + off);
            window.draw(lt);
        }

        // Subtitle slides up
        float slide = max(0.f, 40.f * (1.f - t / 0.6f));
        Text st; st.setFont(font);
        st.setString(subtitles[lvl]);
        st.setCharacterSize(36);
        st.setFillColor(Color(255, 255, 255, a8));
        st.setPosition(W/2.f - st.getGlobalBounds().width/2.f, H/2.f + 18.f + slide);
        window.draw(st);

        if (t > 0.6f)
        {
            float pulse = std::sin(t * 4.f) * 0.3f + 0.7f;
            Text hint; hint.setFont(font);
            hint.setString("Press any key to skip");
            hint.setCharacterSize(16);
            hint.setFillColor(Color(200, 200, 200, static_cast<Uint8>(a8 * pulse * 0.6f)));
            hint.setPosition(W/2.f - hint.getGlobalBounds().width/2.f, H - 60.f);
            window.draw(hint);
        }

        window.display();

        Event ev2;
        while (window.pollEvent(ev2))
            if (ev2.type == Event::KeyPressed || ev2.type == Event::MouseButtonPressed)
                goto doneAnnounce;
    }
    doneAnnounce:;
}

// ─────────────────────────────────────────────────────────────────────────────
void Game::handlePauseMenu()
{
    pauseMenu.show();
    switch (pauseMenu.getSelectedOption())
    {
    case 0: isPaused = false; gameClock.restart(); break;
    case 1: isPaused = false; lives = 0;           break;  // exits to main menu
    case 2: window.close();                        break;
    default: break;
    }
}

// ─────────────────────────────────────────────────────────────────────────────
string Game::getPlayerBadge() const
{
    vector<pair<string, int>> scores;
    ifstream in("highscores.txt");
    string n; int s;
    while (in >> n >> s) scores.emplace_back(n, s);
    sort(scores.begin(), scores.end(),
         [](const pair<string,int> &a, const pair<string,int> &b){ return a.second > b.second; });
    for (int i = 0; i < min((int)scores.size(), 3); i++)
    {
        if (scores[i].first == playerName)
        {
            if (i == 0) return "GOLD";
            if (i == 1) return "SILVER";
            return "BRONZE";
        }
    }
    return "";
}

void Game::saveScore() const
{
    vector<pair<string, int>> scores;
    ifstream in("highscores.txt");
    string n; int s;
    while (in >> n >> s) scores.emplace_back(n, s);
    in.close();
    bool found = false;
    for (auto &e : scores)
        if (e.first == playerName) { e.second = max(e.second, score); found = true; break; }
    if (!found) scores.emplace_back(playerName, score);
    sort(scores.begin(), scores.end(),
         [](const pair<string,int> &a, const pair<string,int> &b){ return a.second > b.second; });
    ofstream out("highscores.txt");
    for (auto &[name, sc] : scores) out << name << " " << sc << "\n";
}

#endif // GAME_H
