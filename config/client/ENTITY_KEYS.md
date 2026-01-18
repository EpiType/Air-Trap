# Valid Entity Keys for Sprite Mappings

This file lists all valid entity keys that can be used in `config/client/sprite_mappings.json`.

## Player & Ships
- `player_ship` - Main player spaceship

## Player Projectiles
- `player_shot_6` - Standard player bullet (used for `player_bullet` tag)

## Enemy Projectiles  
- `enemy_shot_6` - Standard enemy bullet (used for `enemy_bullet` tag)
 - `shot_6` - Legacy key (kept for compatibility)
- `shot_insane` - Fast/dangerous enemy shot
- `shot_1` - Enemy shot type 1
- `shot_2` - Enemy shot type 2
- `shot_3` - Enemy shot type 3
- `shot_4` - Enemy shot type 4
- `shot_5` - Enemy shot type 5

## Effects
- `effect_1` - Visual effect 1
- `effect_2` - Visual effect 2
- `effect_3` - Visual effect 3

## Power-ups
- `power_up_heal` - Health recovery power-up
- `power_up_double` - Double shot power-up
- `power_up_shield` - Shield power-up

## Example sprite_mappings.json

```json
{
  "player_ship": "assets/sprites/custom/players/my_ship.png",
  "shot_6": "assets/sprites/custom/projectiles/my_bullet.png"
}
```

## Notes

- Entity keys are case-sensitive
- Custom sprites should match the original sprite dimensions
- The mod menu automatically resizes sprites to match original dimensions
- Changes require a game restart or reload via mod menu
