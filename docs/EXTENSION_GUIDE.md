# 🎮 Extension Guide

This guide explains how to add new content to Air-Trap: enemies, levels, weapons, power-ups, and systems.

## Table of Contents

1. [Adding a New Enemy](#adding-a-new-enemy)
2. [Adding a New Level](#adding-a-new-level)
3. [Adding a New Component](#adding-a-new-component)
4. [Adding a New System](#adding-a-new-system)
5. [Adding a Power-Up](#adding-a-power-up)
6. [Adding a New Weapon](#adding-a-new-weapon)

---

## Adding a New Enemy

### Step 1: Define the Entity Type

Add the new enemy type to the `EntityType` enum:

```cpp
// common/include/RType/Network/Packet.hpp (around line 145)
enum class EntityType : uint8_t {
    Player = 1,
    Scout  = 2,
    Tank   = 3,
    Boss   = 4,
    // Add your new enemy:
    Bomber = 15,  // New enemy type
    // ...
};
```

### Step 2: Create Sprite Definition (Client)

Add the sprite configuration in `EntityBuilder.hpp`:

```cpp
// client/include/Game/EntityBuilder.hpp

// Add a new SpriteAnimDef for your enemy
static constexpr SpriteAnimDef k_bomber{
    "bomber",                              // ID
    "assets/sprites/enemies/bomber.png",   // Texture path
    0,                                     // rectLeft
    0,                                     // rectTop
    64,                                    // rectWidth
    64,                                    // rectHeight
    5,                                     // zIndex
    255,                                   // red
    255,                                   // green  
    255,                                   // blue
    true,                                  // withAnimation
    64,                                    // frameWidth
    64,                                    // frameHeight
    0,                                     // frameLeft
    0,                                     // frameTop
    4,                                     // totalFrames
    0.15f,                                 // animation speed
    {-1.0f, 0.0f},                        // direction
    {2.0f, 2.0f}                          // scale
};

// Add factory function
static EntityTemplate createBomber(const Vec2f& pos) {
    return makeFromDef(pos, k_bomber);
}
```

### Step 3: Handle Entity Spawn (Client)

Update `NetworkSyncSystem.cpp` to handle the new entity type:

```cpp
// client/src/Systems/NetworkSyncSystem.cpp

void NetworkSyncSystem::handleEntitySpawn(const net::Packet& packet) {
    // ... existing code ...
    
    switch (entityType) {
        // ... existing cases ...
        
        case net::EntityType::Bomber: {
            auto templ = EntityTemplate::createBomber(position);
            spawnEntity(registry, templ, networkId);
            break;
        }
    }
}
```

### Step 4: Add Enemy AI (Server)

Create or modify AI behavior in the server:

```cpp
// server/src/Systems/EnemyAISystem.cpp

void EnemyAISystem::updateBomber(ecs::Registry& registry, 
                                  Entity entity,
                                  float deltaTime) {
    auto& transform = registry.get<Transform>().value().get()[entity];
    auto& velocity = registry.get<Velocity>().value().get()[entity];
    auto& behavior = registry.get<IABehavior>().value().get()[entity];
    
    // Custom bomber behavior: dive towards player
    if (behavior.state == AIState::Diving) {
        velocity.direction = {-1.0f, 0.5f};
        velocity.speed = 200.0f;
    }
}
```

### Step 5: Spawn Enemy in Level (Server)

Add spawn logic in `LevelSystem.cpp`:

```cpp
// server/src/Systems/LevelSystem.cpp

void LevelSystem::spawnWave(int waveNumber) {
    // ... existing code ...
    
    if (waveNumber == 5) {
        // Spawn bombers in wave 5
        spawnEnemy(EntityType::Bomber, {1300.f, 200.f});
        spawnEnemy(EntityType::Bomber, {1300.f, 400.f});
    }
}
```

### Step 6: Add Sprite Asset

Place your sprite in the assets folder:
```
assets/sprites/enemies/bomber.png
```

---

## Adding a New Level

### Step 1: Create Background Assets

Add background images to a new level folder:
```
assets/backgrounds/lvl5/
├── sky.png         (1900x1000 or 3800x1000)
├── mountains.png
├── clouds.png
└── foreground.png
```

### Step 2: Define Parallax Layers (Client)

Add parallax definitions in `EntityBuilder.hpp`:

```cpp
// client/include/Game/EntityBuilder.hpp

// Level 5 Parallax layers
static constexpr ParallaxDef kParallaxLvl5_1{
    "assets/backgrounds/lvl5/sky.png",
    8.0f,                    // scroll speed
    {0.0f, 0.0f},           // velocity direction
    200,                     // opacity
    0.0f,                    // parallax speed modifier
    1900.0f,                 // texture width
    {2.0f, 1.0f}            // scale
};

static constexpr ParallaxDef kParallaxLvl5_2{
    "assets/backgrounds/lvl5/mountains.png",
    15.0f,
    {0.0f, 0.0f},
    255,
    0.0f,
    1900.0f,
    {2.0f, 1.0f}
};

// Add factory function
static std::vector<EntityTemplate> createParallaxLvl5() {
    return {
        makeParallax(kParallaxLvl5_1),
        makeParallax(kParallaxLvl5_2),
        // Add more layers...
    };
}
```

### Step 3: Update PlayingScene (Client)

Modify `PlayingScene.cpp` to spawn new level parallax:

```cpp
// client/src/Scenes/PlayingScene.cpp

void PlayingScene::spawnParallax(int levelId) {
    std::vector<EntityTemplate> layers;
    
    switch (levelId) {
        // ... existing levels ...
        case 5:
            layers = EntityTemplate::createParallaxLvl5();
            break;
    }
    
    for (const auto& layer : layers) {
        spawnParallaxEntity(layer);
    }
}
```

### Step 4: Create Level Configuration (Server)

Add level data in the config:

```json
// common/assets/levels/level5.json
{
    "id": 5,
    "name": "Volcanic Zone",
    "duration": 120,
    "waves": [
        {
            "time": 0,
            "enemies": [
                {"type": "Scout", "x": 1300, "y": 200},
                {"type": "Scout", "x": 1300, "y": 400}
            ]
        },
        {
            "time": 10,
            "enemies": [
                {"type": "Tank", "x": 1300, "y": 300}
            ]
        }
    ],
    "boss": {
        "type": "Boss",
        "spawnTime": 100
    }
}
```

### Step 5: Register Level (Server)

Update `LevelSystem.cpp`:

```cpp
// server/src/Systems/LevelSystem.cpp

void LevelSystem::loadLevel(int levelId) {
    std::string path = "assets/levels/level" + std::to_string(levelId) + ".json";
    // Load and parse JSON...
}
```

---

## Adding a New Component

### Step 1: Create Component Header

```cpp
// common/include/RType/ECS/Components/Poison.hpp
#pragma once

namespace rtp::ecs::components
{
    /**
     * @brief Poison damage over time component
     */
    struct Poison {
        float damagePerSecond = 5.0f;
        float duration = 3.0f;
        float elapsed = 0.0f;
    };
}
```

### Step 2: Register Component

Components must be registered with the Registry before use:

```cpp
// In your initialization code (server or client)
registry.subscribe<ecs::components::Poison>();
```

### Step 3: Use Component

```cpp
// Add to entity
registry.add<Poison>(entity, 10.0f, 5.0f, 0.0f);

// Check and access
if (registry.has<Poison>(entity)) {
    auto& poison = registry.get<Poison>().value().get()[entity];
    poison.elapsed += deltaTime;
}

// Iterate all poisoned entities
for (auto [e, health, poison] : registry.zipView<Health, Poison>()) {
    health.current -= poison.damagePerSecond * deltaTime;
}
```

---

## Adding a New System

### Step 1: Create System Header

```cpp
// server/include/Systems/PoisonSystem.hpp
#pragma once

#include "RType/ECS/Registry.hpp"

namespace rtp::server
{
    class PoisonSystem {
    public:
        void update(ecs::Registry& registry, float deltaTime);
    };
}
```

### Step 2: Implement System

```cpp
// server/src/Systems/PoisonSystem.cpp
#include "Systems/PoisonSystem.hpp"
#include "RType/ECS/Components/Health.hpp"
#include "RType/ECS/Components/Poison.hpp"

namespace rtp::server
{
    void PoisonSystem::update(ecs::Registry& registry, float deltaTime) {
        using namespace ecs::components;
        
        std::vector<ecs::Entity> toRemove;
        
        for (auto [entity, health, poison] : 
             registry.zipView<Health, Poison>()) {
            
            // Apply damage
            health.current -= poison.damagePerSecond * deltaTime;
            poison.elapsed += deltaTime;
            
            // Mark expired poison for removal
            if (poison.elapsed >= poison.duration) {
                toRemove.push_back(entity);
            }
        }
        
        // Remove expired poison components
        for (auto entity : toRemove) {
            registry.remove<Poison>(entity);
        }
    }
}
```

### Step 3: Add to Game Loop

```cpp
// server/src/Game/GameLoop.cpp

#include "Systems/PoisonSystem.hpp"

void GameLoop::update(float deltaTime) {
    // ... other systems ...
    
    _poisonSystem.update(_registry, deltaTime);
    
    // ... more systems ...
}
```

### Step 4: Update CMakeLists.txt

```cmake
# server/CMakeLists.txt
target_sources(r-type_server PRIVATE
    # ... existing sources ...
    src/Systems/PoisonSystem.cpp
)
```

---

## Adding a Power-Up

### Step 1: Define Entity Type

```cpp
// common/include/RType/Network/Packet.hpp
enum class EntityType : uint8_t {
    // ... existing ...
    PowerupPoison = 16,  // New power-up
};
```

### Step 2: Create Component (if needed)

```cpp
// common/include/RType/ECS/Components/Powerup.hpp
struct Powerup {
    enum class Type : uint8_t {
        Heal = 0,
        Shield = 1,
        DoubleFire = 2,
        Poison = 3  // New type
    };
    
    Type type;
    float value;
};
```

### Step 3: Add Sprite (Client)

```cpp
// client/include/Game/EntityBuilder.hpp
static constexpr SpriteAnimDef k_powerup_poison{
    "powerup_poison",
    "assets/sprites/powerups/poison.png",
    0, 0, 32, 32,
    10, 255, 200, 50,  // Green-ish color
    true, 32, 32, 0, 0, 4, 0.1f,
    {0.0f, 0.0f}, {1.5f, 1.5f}
};
```

### Step 4: Handle Collection (Server)

```cpp
// server/src/Systems/CollisionSystem.cpp

void CollisionSystem::handlePlayerPowerupCollision(
    Entity player, Entity powerup) {
    
    auto& pu = _registry.get<Powerup>().value().get()[powerup];
    
    switch (pu.type) {
        case Powerup::Type::Poison:
            // Give player poison ability
            _registry.add<PoisonWeapon>(player, 5.0f, 3.0f);
            break;
        // ... other types ...
    }
    
    _registry.kill(powerup);
}
```

---

## Adding a New Weapon

### Step 1: Create Weapon Component

```cpp
// common/include/RType/ECS/Components/LaserWeapon.hpp
#pragma once

namespace rtp::ecs::components
{
    struct LaserWeapon {
        float damage = 50.0f;
        float range = 800.0f;
        float cooldown = 0.5f;
        float cooldownTimer = 0.0f;
        bool firing = false;
    };
}
```

### Step 2: Create Weapon System (Server)

```cpp
// server/src/Systems/LaserWeaponSystem.cpp

void LaserWeaponSystem::update(ecs::Registry& registry, float deltaTime) {
    for (auto [entity, laser, transform] : 
         registry.zipView<LaserWeapon, Transform>()) {
        
        laser.cooldownTimer -= deltaTime;
        
        if (laser.firing && laser.cooldownTimer <= 0) {
            fireLaser(registry, entity, transform);
            laser.cooldownTimer = laser.cooldown;
        }
    }
}

void LaserWeaponSystem::fireLaser(ecs::Registry& registry, 
                                   Entity shooter,
                                   const Transform& origin) {
    // Raycast to find hit target
    // Apply damage directly (no projectile entity)
    // Send visual effect to clients
}
```

### Step 3: Handle Input (Server)

```cpp
// server/src/Systems/PlayerShootSystem.cpp

void PlayerShootSystem::handleInput(Entity player, 
                                     const InputPayload& input) {
    if (registry.has<LaserWeapon>(player)) {
        auto& laser = registry.get<LaserWeapon>().value().get()[player];
        laser.firing = input.shooting;
    }
}
```

### Step 4: Add Visual Effect (Client)

```cpp
// client/src/Systems/RenderSystem.cpp

void RenderSystem::drawLaserBeams(sf::RenderTarget& target) {
    for (auto [entity, laser, transform] : 
         _registry.zipView<LaserWeapon, Transform>()) {
        
        if (laser.firing) {
            sf::RectangleShape beam({laser.range, 4.f});
            beam.setPosition(transform.position.x, transform.position.y);
            beam.setFillColor(sf::Color::Red);
            target.draw(beam);
        }
    }
}
```

---

## Best Practices

### 1. Component Design
- Keep components as pure data (no logic)
- Use small, focused components
- Prefer composition over inheritance

### 2. System Design
- One responsibility per system
- Systems should be stateless when possible
- Use dependency injection for testing

### 3. Network Considerations
- New entity types need corresponding `EntityType` enum values
- Consider bandwidth when adding new synchronized data
- Use UDP for frequent updates, TCP for critical events

### 4. Asset Guidelines
- Background images: 1900x1000 or 3800x1000 (seamless horizontal)
- Sprite sheets: Power of 2 dimensions preferred
- Use PNG format with transparency

### 5. Testing
- Add unit tests for new components
- Test system behavior with mock registries
- Verify network serialization/deserialization

---

## File Checklist for New Content

### New Enemy
- [ ] `common/.../Packet.hpp` - EntityType enum
- [ ] `client/.../EntityBuilder.hpp` - SpriteAnimDef + factory
- [ ] `client/.../NetworkSyncSystem.cpp` - spawn handler
- [ ] `server/.../EnemyAISystem.cpp` - behavior logic
- [ ] `server/.../LevelSystem.cpp` - spawn locations
- [ ] `assets/sprites/enemies/` - sprite file

### New Level
- [ ] `assets/backgrounds/lvlN/` - background images
- [ ] `client/.../EntityBuilder.hpp` - parallax defs
- [ ] `client/.../PlayingScene.cpp` - parallax spawning
- [ ] `common/assets/levels/` - level config JSON
- [ ] `server/.../LevelSystem.cpp` - level loading

### New Component
- [ ] `common/.../Components/` - component header
- [ ] Registry subscription in initialization
- [ ] Related system(s) updated

### New System
- [ ] Header in `include/Systems/`
- [ ] Implementation in `src/Systems/`
- [ ] CMakeLists.txt updated
- [ ] Added to game loop
