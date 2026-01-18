# 🎮 Client Module

The client module handles all player-facing functionality: rendering, input, audio, and UI.

## Directory Structure

```
client/
├── include/
│   ├── Audio/          # Audio management
│   ├── Core/           # Application lifecycle
│   ├── Game/           # Entity templates and builders
│   ├── Interfaces/     # Abstract interfaces
│   ├── Network/        # Client networking
│   ├── Scenes/         # Game scenes (state machine)
│   ├── Systems/        # Client-side ECS systems
│   ├── Translation/    # i18n support
│   └── UI/             # User interface components
├── src/                # Implementation files
└── CMakeLists.txt      # Build configuration
```

## Key Components

### Core/Application

The main application class managing the game loop:

```cpp
class Application {
public:
    void run();           // Main loop
    void changeState();   // Scene transitions
private:
    sf::RenderWindow _window;
    ecs::Registry _registry;
    std::unique_ptr<IScene> _currentScene;
};
```

### Scenes

Scenes represent different game states:

| Scene | Description |
|-------|-------------|
| `LoginScene` | User authentication |
| `LobbyScene` | Room browsing |
| `CreateRoomScene` | Room creation |
| `RoomWaitingScene` | Pre-game lobby |
| `PlayingScene` | Main gameplay |
| `PauseScene` | Pause overlay |
| `SettingsScene` | Configuration |
| `MenuScene` | Main menu |

### Systems

Client systems process visual and audio updates:

| System | Description |
|--------|-------------|
| `RenderSystem` | Draw sprites to screen |
| `AnimationSystem` | Update sprite animations |
| `InputSystem` | Capture keyboard/gamepad |
| `AudioSystem` | Play sounds and music |
| `UISystem` | Handle UI interactions |
| `NetworkSyncSystem` | Process server packets |
| `ParallaxSystem` | Scrolling backgrounds |

### Network

The `ClientNetwork` class handles server communication:

```cpp
class ClientNetwork {
public:
    void sendPacket(const Packet& packet, NetworkMode mode);
    std::optional<NetworkEvent> pollEvent();
};
```

## Dependencies

- **SFML 3.0.2**: Graphics, window, audio
- **ASIO 1.28.0**: Networking
- **RTypeCommon**: Shared ECS and protocol

## Building

```bash
cmake --build build --target r-type_client
```

## Usage

```bash
./r-type_client <server_ip> <port>
```
