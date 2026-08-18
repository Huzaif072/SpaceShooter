# 🚀 SpaceShooter

A fully-featured **3-level space shooter** built with **C++ and SFML 2** for macOS, developed as a Semester 2 OOP project.

---

## 🎮 Gameplay

| Feature | Details |
|---|---|
| **Levels** | 3 levels × 3 phases each |
| **Enemies** | Alpha (Red), Beta (Blue), Gamma (Green) — random sprite variants |
| **Boss enemies** | Monster (mechanical orb) and Dragon — spawn between phases |
| **Player ship** | Moves in all 4 directions, wraps horizontally |
| **Weapons** | Normal laser, 7-way spread (power-up), fire-mode green laser |
| **Power-ups** | ⚡ Power Up, 🌟 Fire Mode, 💊 Extra Life, ☠️ Danger (avoid!) |
| **Scoring** | Points scale per level; high-score leaderboard with GOLD/SILVER/BRONZE badges |

## ✨ Visual Features

- 🌌 Milky Way galaxy background
- 🪐 18 rotating meteors drifting in the background
- 💥 Explosion animations on every kill
- 🔴 Screen-flash hit indicator
- 🛡️ Pulsing shield overlay during Power-Up
- 🎬 Per-level animated announcement screen (particle burst, pulsing rings, subtitle slide)
- 🖥️ Glassmorphism HUD — score, timer, level/phase, life icons, badge

---

## 🕹️ Controls

| Key | Action |
|---|---|
| `←` `→` `↑` `↓` | Move spaceship |
| `Space` | Fire |
| `Esc` | Pause / Resume |

---

## 🛠️ Building (macOS)

### Prerequisites

Install SFML 2 via Homebrew:
```bash
brew install sfml@2
```

### Build & Run

```bash
make -f MakeFile bundle
```

This compiles the game and launches `SpaceShooter.app`.

---

## 📁 Project Structure

```
Spaceshooter/
├── coding/             # All C++ source headers
│   ├── game.h          # Main game loop, HUD, collision, spawning
│   ├── spaceship.h     # Player ship — movement, firing, shield
│   ├── invaders.h      # Enemy invaders (Alpha/Beta/Gamma)
│   ├── bullet.h        # Player bullets (delta-time based)
│   ├── bomb.h          # Enemy bombs (delta-time based)
│   ├── addon.h         # Power-up items (real sprites + pulse animation)
│   ├── explosion.h     # Explosion fade-out effect
│   ├── meteor.h        # Background rotating meteors
│   ├── monster_enemy.h # Boss: Monster (lightning bolts)
│   ├── dragon_enemy.h  # Boss: Dragon (spread fire)
│   ├── menu.h          # Main menu
│   ├── pausemenu.h     # Pause menu
│   ├── gameover.h      # Game-over screen
│   └── endscreen.h     # Level-complete / win screen
├── img/                # All PNG and JPG assets
│   ├── PNG/
│   │   ├── Enemies/    # enemyRed/Blue/Green 1-4
│   │   ├── Lasers/     # laserBlue/Green/Red
│   │   ├── Power-ups/  # powerup sprites
│   │   ├── Meteors/    # meteor sprites
│   │   ├── Effects/    # shield, fire, speed
│   │   └── UI/         # life icons, buttons, numerals
│   ├── background1.jpg # Milky Way background
│   ├── Spaceship2.png  # Player ship
│   ├── dragon2.png     # Dragon boss
│   ├── monster.png     # Monster boss
│   └── ...
├── Fonts/              # Font files (ariblk.ttf required)
├── MakeFile            # Build script (macOS + SFML 2)
├── Info.plist          # macOS .app bundle metadata
└── README.md
```

---

## 🏗️ Architecture

The game uses an **OOP design** with the following key classes:

- `Game` — central controller: game loop, state management, collisions, HUD
- `Spaceship` — player entity with event-driven keyboard input (reliable on macOS)
- `Enemy` (abstract) → `Invader` → `AlphaInvader / BetaInvader / GammaInvader`
- `MonsterEnemy` / `DragonEnemy` — boss entities with unique attack patterns
- `Bullet` / `Bomb` / `AddOn` / `Explosion` / `Meteor` — independent game entities

All movement is **delta-time based** for frame-rate independence.

---

## 📜 License

MIT — free to use and modify for educational purposes.
