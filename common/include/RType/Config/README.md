# Configuration Architecture

This document describes the organization of configuration files and parsers in the Air-Trap project.

## Directory Structure

```
common/include/RType/Config/
├── SimpleJsonParser.hpp    # Low-level JSON parser (no external dependencies)
├── WeaponConfig.hpp         # Weapon definitions loader
└── SpriteMapConfig.hpp      # Sprite mapping loader (mod system)

config/
├── common/                  # Shared configs (server + client)
│   └── weapons.json         # Weapon stats and properties
└── client/                  # Client-only configs
    ├── sprite_mappings.json # Custom sprite mod mappings
    └── ENTITY_KEYS.md       # Valid entity keys documentation
```

## Parser Layer (common/include/RType/Config/)

All configuration parsers are centralized in the `common/` directory to ensure consistency and reusability.

### SimpleJsonParser.hpp
- **Purpose**: Lightweight JSON parser with no external dependencies
- **API**:
  - `bool parse(filepath)` - Parse JSON from file path
  - `bool parseContent(jsonString)` - Parse JSON from string
  - `getData()` - Get parsed key-value pairs
  - `getString()`, `getInt()`, `getFloat()`, `getBool()` - Type-safe accessors

### WeaponConfig.hpp
- **Purpose**: Load and manage weapon definitions
- **API**:
  - `loadWeaponConfigs()` - Load all weapons from config/common/weapons.json
  - `getWeaponDef(WeaponKind)` - Get weapon stats by type
  - `hasWeaponConfigs()` - Check if configs loaded successfully
  - `getWeaponDisplayName(WeaponKind)` - Get localized weapon name

### SpriteMapConfig.hpp
- **Purpose**: Load and save sprite mappings for the mod system
- **API**:
  - `loadSpriteMappings(filepath)` - Load sprite mappings from JSON
  - `saveSpriteMappings(mappings, filepath)` - Save sprite mappings to JSON
- **Returns**: `std::unordered_map<std::string, std::string>` (entity key → sprite path)

## Usage Examples

### Loading Weapon Configs (Server/Client)
```cpp
#include "RType/Config/WeaponConfig.hpp"

// Load all weapon definitions
rtp::config::loadWeaponConfigs();

// Get specific weapon stats
auto weapon = rtp::config::getWeaponDef(WeaponKind::Classic);
int damage = weapon.damage;
float fireRate = weapon.fireRate;
```

### Loading Sprite Mappings (Client Only)
```cpp
#include "RType/Config/SpriteMapConfig.hpp"

// Load sprite mappings
auto mappings = rtp::config::loadSpriteMappings("config/client/sprite_mappings.json");

// Check for custom sprite
if (mappings.find("shot_6") != mappings.end()) {
    std::string customPath = mappings["shot_6"];
}
```

### Saving Sprite Mappings (Client Only)
```cpp
#include "RType/Config/SpriteMapConfig.hpp"

std::unordered_map<std::string, std::string> mappings;
mappings["shot_6"] = "assets/sprites/custom/projectiles/my_bullet.png";

bool success = rtp::config::saveSpriteMappings(mappings, "config/client/sprite_mappings.json");
```

## Client Integration

Client code should use the high-level config loaders instead of direct JSON parsing:

- **SpriteCustomizer**: Uses `SpriteMapConfig::loadSpriteMappings()`
- **ModMenuScene**: Uses `SpriteMapConfig::loadSpriteMappings()` and `saveSpriteMappings()`
- **EntitySystem**: Uses `WeaponConfig::loadWeaponConfigs()` and `getWeaponDef()`

## Benefits of Centralization

1. **Single Source of Truth**: All JSON parsing logic in one place
2. **Consistency**: Same parser behavior across server and client
3. **No External Dependencies**: Custom parser means no nlohmann/json requirement
4. **Type Safety**: Structured loaders with clear APIs
5. **Maintainability**: Easy to update parsing logic without touching client code
6. **Testability**: Config loaders can be tested independently

## Adding New Config Types

To add a new configuration type:

1. Create a new `*Config.hpp` file in `common/include/RType/Config/`
2. Use `SimpleJsonParser` for parsing
3. Provide `load*()` and optionally `save*()` functions
4. Return structured data (maps, vectors, or custom structs)
5. Document the config file format and location

Example:
```cpp
// common/include/RType/Config/EnemyConfig.hpp
#include "SimpleJsonParser.hpp"

namespace rtp::config {
    inline std::vector<EnemyDef> loadEnemyConfigs(
        const std::string& filepath = "config/common/enemies.json"
    ) {
        // Implementation using SimpleJsonParser
    }
}
```
