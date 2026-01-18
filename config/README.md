# Configuration Files

This directory contains all configuration files for the Air-Trap project.

## Directory Structure

```
config/
├── common/           # Shared configs (server + client)
│   └── weapons.json  # Weapon definitions and stats
│
└── client/           # Client-only configs
    ├── sprite_mappings.json  # Custom sprite mod mappings
    └── ENTITY_KEYS.md        # Valid entity keys documentation
```

## Common Configuration (`config/common/`)

These configuration files are used by both the server and client:

- **`weapons.json`**: Defines all weapon types with their stats (damage, fire rate, ammo, special abilities)
  - Used by server for gameplay mechanics
  - Used by client for UI display and entity rendering

## Client Configuration (`config/client/`)

These configuration files are specific to the client:

- **`sprite_mappings.json`**: Maps entity names to custom sprite file paths for the mod system
  - Managed through the in-game Mod Menu
  - Allows players to customize entity appearances
  - Format: `{"entity_name": "path/to/custom/sprite.png"}`
  - See `ENTITY_KEYS.md` for valid entity keys

## Configuration Loaders

All configuration files use centralized loaders in `common/include/RType/Config/`:

- **SimpleJsonParser.hpp**: Custom lightweight JSON parser (no external dependencies)
- **WeaponConfig.hpp**: Weapon definitions loader
- **SpriteMapConfig.hpp**: Sprite mapping loader

See `common/include/RType/Config/README.md` for detailed architecture documentation.

## Adding New Configurations

- **Game mechanics** → Add to `config/common/` (create loader in `common/include/RType/Config/`)
- **Client visuals/UI** → Add to `config/client/` (use existing loaders or create new ones)

