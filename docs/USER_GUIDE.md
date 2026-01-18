# 📖 User Guide

Welcome to Air-Trap, a multiplayer shoot 'em up game inspired by the classic R-Type!

## Table of Contents

1. [Getting Started](#getting-started)
2. [Starting the Server](#starting-the-server)
3. [Connecting with the Client](#connecting-with-the-client)
4. [Game Controls](#game-controls)
5. [Gameplay](#gameplay)
6. [Multiplayer](#multiplayer)
7. [Settings](#settings)
8. [Troubleshooting](#troubleshooting)

---

## Getting Started

### System Requirements

| Component | Minimum | Recommended |
|-----------|---------|-------------|
| OS | Ubuntu 20.04 / macOS 11 | Ubuntu 22.04+ / macOS 13+ |
| CPU | Dual-core 2.0 GHz | Quad-core 3.0 GHz |
| RAM | 2 GB | 4 GB |
| Graphics | OpenGL 3.3 | OpenGL 4.5 |
| Network | Broadband connection | Low-latency connection |

### Installation

If you have pre-built binaries:
```bash
# Extract the archive
tar -xzf air-trap-linux.tar.gz
cd air-trap

# Make executables runnable
chmod +x r-type_server r-type_client
```

If building from source, see the [README.md](../README.md).

---

## Starting the Server

The server hosts the game and manages all game logic.

### Basic Usage

```bash
./r-type_server
```

By default, the server starts on **port 5000**.

### Server Output

```
[INFO] Starting R-Type Server on port 5000
[INFO] Server ready, waiting for connections...
```

### Server Configuration

Edit `config/server.json` to customize:

```json
{
    "port": 5000,
    "maxPlayers": 4,
    "tickRate": 60,
    "logLevel": "INFO"
}
```

### Stopping the Server

Press `Ctrl+C` in the terminal to gracefully stop the server.

---

## Connecting with the Client

### Basic Usage

```bash
./r-type_client <server_ip> <server_port>
```

### Examples

**Local Game (same computer):**
```bash
./r-type_client 127.0.0.1 5000
```

**LAN Game:**
```bash
./r-type_client 192.168.1.100 5000
```

**Internet Game:**
```bash
./r-type_client game.example.com 5000
```

### Login Screen

1. Enter your **username** (3-16 characters)
2. Enter your **password**
3. Click **Login** or **Register** if new player

### Lobby

After logging in, you'll see the lobby:

1. **Room List**: Shows available game rooms
2. **Create Room**: Start a new game room
3. **Join Room**: Enter an existing room
4. **Settings**: Adjust game settings

---

## Game Controls

### Keyboard Controls

| Action | Primary | Alternative |
|--------|---------|-------------|
| Move Up | ↑ Arrow | W |
| Move Down | ↓ Arrow | S |
| Move Left | ← Arrow | A |
| Move Right | → Arrow | D |
| Fire | Space | Left Ctrl |
| Charge Shot | Hold Space | Hold Left Ctrl |
| Pause | Escape | P |

### Gamepad Controls

| Action | Button |
|--------|--------|
| Move | Left Stick / D-Pad |
| Fire | A / X |
| Charge Shot | Hold A / Hold X |
| Pause | Start |
| Back/Cancel | B / Circle |

### Customizing Controls

1. Go to **Settings** from the menu
2. Select **Key Bindings** or **Gamepad Settings**
3. Click on the action you want to change
4. Press the new key/button
5. Click **Save**

---

## Gameplay

### Objective

Survive waves of enemies, defeat bosses, and achieve the highest score!

### Player Ship

Your ship can:
- Move in all 4 directions
- Fire standard shots (tap fire button)
- Charge powerful shots (hold fire button)

### Enemies

| Enemy | Description | Threat Level |
|-------|-------------|--------------|
| **Scout** | Fast, weak enemies | ⭐ |
| **Tank** | Slow, armored enemies | ⭐⭐ |
| **Boss** | End-of-level challenges | ⭐⭐⭐⭐⭐ |

### Power-Ups

Collect power-ups dropped by enemies:

| Power-Up | Color | Effect |
|----------|-------|--------|
| **Health** | Red | Restores HP |
| **Shield** | Green | Blocks 1 hit |
| **Double Fire** | Yellow | Fires 2 shots for 20s |

### Levels

The game features 4 unique levels:

1. **Level 1 - Space**: Classic space background with asteroids
2. **Level 2 - Moon**: Lunar surface with craters
3. **Level 3 - Sky**: Clouds and mountains
4. **Level 4 - Desert**: Sandy dunes and pyramids

### Scoring

| Action | Points |
|--------|--------|
| Destroy Scout | 100 |
| Destroy Tank | 250 |
| Destroy Boss | 1000 |
| Collect Power-Up | 50 |
| Complete Level | 500 |

---

## Multiplayer

### Creating a Room

1. Click **Create Room** in the lobby
2. Enter a room name
3. Wait for other players to join
4. Click **Ready** when prepared
5. Host clicks **Start Game** when all ready

### Joining a Room

1. Browse the room list in the lobby
2. Click on a room to select it
3. Click **Join**
4. Click **Ready** when prepared

### In-Game Chat

Press **Enter** to open chat, type your message, press **Enter** again to send.

### Co-op Gameplay

- All players share the same level
- Each player controls their own ship
- Team up to defeat bosses faster
- Compete for the highest individual score

---

## Settings

### Audio

| Setting | Description |
|---------|-------------|
| Master Volume | Overall volume |
| Music Volume | Background music |
| SFX Volume | Sound effects |
| Mute | Toggle all audio |

### Graphics

| Setting | Description |
|---------|-------------|
| Fullscreen | Toggle windowed/fullscreen |
| VSync | Synchronize with monitor |
| Resolution | Screen resolution |

### Accessibility

| Setting | Description |
|---------|-------------|
| Colorblind Mode | Adjusted color palette |
| Screen Shake | Toggle camera shake effects |
| High Contrast | Enhanced visibility |

### Language

Supported languages:
- English (en)
- Français (fr)
- Deutsch (de)
- Español (es)
- Italiano (it)

---

## Troubleshooting

### Connection Issues

**"Could not connect to server"**

1. Verify the server is running
2. Check the IP address and port
3. Ensure no firewall is blocking port 5000
4. Try using `127.0.0.1` for local testing

**"Connection lost"**

1. Check your internet connection
2. The server may have crashed - restart it
3. Reduce network congestion

### Graphics Issues

**Black screen on startup**

1. Update your graphics drivers
2. Try windowed mode: edit `config/client.json`, set `"fullscreen": false`
3. Ensure OpenGL 3.3+ is supported

**Low FPS**

1. Close other applications
2. Lower resolution in settings
3. Enable VSync

### Audio Issues

**No sound**

1. Check system volume
2. Verify audio device is connected
3. Check in-game audio settings

### Linux-Specific Issues

**Missing libraries**

```bash
# Set library path for GCC 14
export LD_LIBRARY_PATH=/usr/local/lib64:$LD_LIBRARY_PATH
./r-type_client 127.0.0.1 5000
```

**Permission denied**

```bash
chmod +x r-type_client r-type_server
```

### Port Already in Use

**"Address already in use" error**

```bash
# Find process using port 5000
lsof -i :5000

# Kill the process (replace PID)
kill -9 <PID>

# Or use a different port
./r-type_server --port 5001
```

---

## FAQ

**Q: How many players can join a game?**
A: Up to 4 players per room.

**Q: Can I play offline?**
A: No, the game requires a server connection even for single-player.

**Q: Are there saves?**
A: Progress is saved server-side. Your account retains high scores.

**Q: How do I host for friends over the internet?**
A: Port forward port 5000 on your router, then share your public IP.

**Q: Can I use a controller?**
A: Yes! Both Xbox and PlayStation controllers are supported.

---

## Support

For bug reports and feature requests, please open an issue on the GitHub repository.

---

Enjoy playing Air-Trap! 🚀
