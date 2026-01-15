# Mod Menu - Custom Sprites Guide

## Overview

The Mod Menu feature allows you to customize the game by replacing default sprites with your own custom images. This includes player ships, enemies, projectiles, and effects.

## How to Access

1. Launch the game client
2. From the main menu, click on **"MODS"** button
3. You'll see the Mod Menu with different sprite categories

## Directory Structure

When you first access the Mod Menu, the system automatically creates the following directory structure:

```
assets/sprites/custom/
├── README.txt
├── players/
├── enemies/
├── projectiles/
└── effects/
```

## Customizing Sprites

### Step 1: Prepare Your Sprites

- **Format**: PNG (recommended for transparency) or JPG
- **Size**: Match the original sprite dimensions for best results
- **Background**: Use transparent backgrounds for PNG files

### Step 2: Place Your Sprites

Copy your custom sprite files into the appropriate category folder:

- **players/** - Custom player ship sprites
- **enemies/** - Custom enemy ship sprites  
- **projectiles/** - Custom bullet and missile sprites
- **effects/** - Custom explosion and effect sprites

### Step 3: Apply Changes

Currently, you need to manually copy sprites to the directory. The file browser feature is planned for a future update.

## Categories

### Players
Customize the appearance of your player ship and different player characters.

### Enemies
Replace enemy ship sprites with your own designs.

### Projectiles
Modify the look of bullets, missiles, and other projectiles.

### Effects
Change explosion effects, particle effects, and other visual effects.

## Tips

- Keep original sprites as backup before replacing them
- Test your custom sprites in-game to ensure they work correctly
- Use consistent art style across all custom sprites for better visual coherence
- Transparent backgrounds (PNG) work best for most sprite types

## Future Features

The following features are planned for future releases:

- [ ] Built-in file browser for easy sprite selection
- [ ] Live preview of custom sprites
- [ ] Sprite size validation and auto-scaling
- [ ] Save/load custom sprite configurations
- [ ] Share sprite packs with other players
- [ ] Hot-reload sprites without restarting the game

## Troubleshooting

**My custom sprites don't appear in the game:**
- Ensure sprites are in the correct category folder
- Check that file format is PNG or JPG
- Verify file names match expected sprite names
- Restart the game after adding new sprites

**Sprites look distorted:**
- Check sprite dimensions match the original
- Ensure aspect ratio is correct
- Try using PNG format with transparency

## Support

For issues or questions about the Mod Menu, please check the main project documentation or create an issue on the project repository.
