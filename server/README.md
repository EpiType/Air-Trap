# 🖥️ Server Module

The server module implements the authoritative game server, handling all game logic, entity management, and client synchronization.

## Directory Structure

```
server/
├── include/
│   ├── Game/           # Game logic and room management
│   ├── ServerNetwork/  # Server networking
│   └── Systems/        # Server-side ECS systems
├── src/
│   ├── main.cpp        # Entry point
│   ├── Game/           # Game implementation
│   ├── ServerNetwork/  # Network implementation
│   └── Systems/        # System implementations
└── CMakeLists.txt      # Build configuration
```

## Key Components

### ServerNetwork

Handles TCP and UDP connections:

```cpp
class ServerNetwork {
public:
    ServerNetwork(uint16_t port);
    
    void start();
    void stop();
    
    void sendPacket(uint32_t sessionId, const Packet& packet, NetworkMode mode);
    void broadcastPacket(const Packet& packet, NetworkMode mode);
    
    std::optional<NetworkEvent> pollEvent();
};
```

### Game/Room

Manages individual game rooms:

```cpp
class Room {
public:
    void addPlayer(uint32_t sessionId);
    void removePlayer(uint32_t sessionId);
    void startGame();
    void update(float deltaTime);
    
private:
    ecs::Registry _registry;
    std::vector<uint32_t> _players;
    int _currentLevel;
};
```

### Systems

Server systems implement game logic:

| System | Description |
|--------|-------------|
| `MovementSystem` | Entity movement physics |
| `CollisionSystem` | Collision detection and response |
| `EnemyAISystem` | Enemy behavior and pathfinding |
| `EnemyShootSystem` | Enemy firing patterns |
| `PlayerShootSystem` | Player weapon handling |
| `BulletCleanupSystem` | Remove off-screen projectiles |
| `LevelSystem` | Level progression and spawning |
| `NetworkSyncSystem` | Broadcast state to clients |
| `PlayerSystem` | Player state management |
| `AuthSystem` | Authentication handling |
| `RoomSystem` | Room management |

## Game Loop

```cpp
void GameLoop::run() {
    while (running) {
        float dt = calculateDeltaTime();
        
        // Process network events
        processNetworkEvents();
        
        // Update all rooms
        for (auto& room : _rooms) {
            room.update(dt);
        }
        
        // Broadcast state
        broadcastState();
        
        // Maintain tick rate
        sleepUntilNextTick();
    }
}
```

## Configuration

Server settings in `config/server.json`:

```json
{
    "port": 5000,
    "maxPlayers": 4,
    "tickRate": 60,
    "maxRooms": 10,
    "logLevel": "INFO"
}
```

## Dependencies

- **ASIO 1.28.0**: Networking
- **RTypeCommon**: Shared ECS and protocol

## Building

```bash
cmake --build build --target r-type_server
```

## Usage

```bash
./r-type_server
# Server starts on port 5000 by default
```

## Network Protocol

### Client → Server

| OpCode | Name | Description |
|--------|------|-------------|
| 0x01 | Hello | Initial connection |
| 0x1A | LoginRequest | Authentication |
| 0x10 | InputTick | Player input state |
| 0x06 | CreateRoom | Create game room |
| 0x07 | JoinRoom | Join existing room |

### Server → Client

| OpCode | Name | Description |
|--------|------|-------------|
| 0x02 | Welcome | Connection accepted |
| 0x21 | EntitySpawn | New entity created |
| 0x22 | EntityDeath | Entity destroyed |
| 0x23 | AmmoUpdate | Ammo state change |
| 0x2B | HealthUpdate | Health change |
