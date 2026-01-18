# 🧑‍💻 Developer Documentation

## Table of Contents

1. [Architecture Overview](#architecture-overview)
2. [ECS Engine](#ecs-engine)
3. [Network Layer](#network-layer)
4. [Client/Server Separation](#clientserver-separation)
5. [Building & Testing](#building--testing)
6. [Code Style](#code-style)

---

## Architecture Overview

Air-Trap follows a **Client-Server architecture** with an **authoritative server** model:

```
┌─────────────────────────────────────────────────────────────────┐
│                         SERVER                                   │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────────┐ │
│  │   Network   │  │    Game     │  │        Systems          │ │
│  │   Layer     │◄─┤    State    │◄─┤ (Collision, AI, etc.)   │ │
│  │ (TCP + UDP) │  │  (Registry) │  │                         │ │
│  └──────┬──────┘  └─────────────┘  └─────────────────────────┘ │
└─────────┼───────────────────────────────────────────────────────┘
          │ Packets (EntitySpawn, EntityDeath, StateUpdate)
          ▼
┌─────────────────────────────────────────────────────────────────┐
│                         CLIENT                                   │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────────┐ │
│  │   Network   │  │   Local     │  │        Systems          │ │
│  │   Layer     │──┤   State     │──┤ (Render, Audio, Input)  │ │
│  │ (TCP + UDP) │  │  (Registry) │  │                         │ │
│  └─────────────┘  └─────────────┘  └─────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

### Key Principles

1. **Server is Authoritative**: All game logic runs on the server
2. **Client is a View**: Client renders state received from server
3. **Input Forwarding**: Client sends inputs, server processes them
4. **Entity Synchronization**: Server broadcasts entity states via UDP

---

## ECS Engine

The ECS (Entity-Component-System) engine is the core of Air-Trap, located in `common/include/RType/ECS/`.

### Entity

An entity is a unique 64-bit identifier combining an index and generation counter:

```cpp
// common/include/RType/ECS/Entity.hpp
namespace rtp::ecs
{
    class Entity {
    public:
        static constexpr std::uint32_t MAX_INDEX = 
            std::numeric_limits<std::uint32_t>::max();
        
        constexpr Entity(std::uint32_t index, std::uint32_t generation);
        
        [[nodiscard]] constexpr std::uint32_t index() const;
        [[nodiscard]] constexpr std::uint32_t generation() const;
        [[nodiscard]] constexpr bool isNull() const noexcept;
        
    private:
        std::uint64_t _id;
    };
}
```

**Generation Counter**: When an entity is killed and its index recycled, the generation increments. This prevents stale entity references from being valid.

### Components

Components are pure data structures. They must satisfy the `Component` concept:

```cpp
// common/include/RType/ECS/ComponentConcept.hpp
template <typename T>
concept Component = std::is_default_constructible_v<T> 
                 && std::is_move_constructible_v<T>;
```

**Available Components** (`common/include/RType/ECS/Components/`):

| Component | Description |
|-----------|-------------|
| `Transform` | Position, rotation, scale |
| `Velocity` | Speed and direction |
| `Sprite` | Texture, rect, z-index, color |
| `Animation` | Frame-based animation data |
| `Health` | HP, max HP |
| `Damage` | Damage value for projectiles |
| `Hitbox` | Collision bounds |
| `NetworkId` | Server-assigned network identifier |
| `Controllable` | Marks entity as player-controlled |
| `IABehavior` | Enemy AI configuration |
| `ParallaxLayer` | Background parallax data |

**Example Component**:
```cpp
// common/include/RType/ECS/Components/Health.hpp
namespace rtp::ecs::components
{
    struct Health {
        int current = 100;
        int max = 100;
    };
}
```

### SparseArray

Components are stored in `SparseArray<T>`, an optimized sparse set data structure:

```cpp
// common/include/RType/ECS/SparseArray.hpp
template <Component T>
class SparseArray final : public ISparseArray {
public:
    // Add component to entity
    template <typename... Args>
    T& emplace(Entity entity, Args&&... args);
    
    // Remove component from entity
    void erase(Entity entity) noexcept;
    
    // Check if entity has component
    [[nodiscard]] bool has(Entity entity) const noexcept;
    
    // Access component (asserts if missing)
    auto&& operator[](Entity entity) noexcept;
    
    // Iterate all components
    [[nodiscard]] std::span<T> data() noexcept;
    [[nodiscard]] std::span<Entity> entities() noexcept;
    
private:
    std::vector<size_t> _sparse;  // Entity index → data index
    std::vector<Entity> _dense;   // Reverse lookup
    std::vector<T> _data;         // Actual component data
};
```

**Complexity**:
- Access by entity: O(1)
- Insert: O(1) amortized
- Erase: O(1) (swap with last)
- Iteration: O(n) where n = active components

### Registry

The `Registry` is the central manager for entities and components:

```cpp
// common/include/RType/ECS/Registry.hpp
namespace rtp::ecs
{
    class Registry {
    public:
        // Entity management
        auto spawn() -> std::expected<Entity, rtp::Error>;
        void kill(Entity entity);
        [[nodiscard]] bool isAlive(Entity entity) const noexcept;
        
        // Component registration (must be called before use)
        template <Component T>
        auto subscribe() -> std::expected<SparseArray<T>&, rtp::Error>;
        
        // Component operations
        template <Component T, typename... Args>
        auto add(Entity entity, Args&&... args) 
            -> std::expected<T&, rtp::Error>;
        
        template <Component T>
        [[nodiscard]] bool has(Entity entity) const noexcept;
        
        template <Component T>
        void remove(Entity entity) noexcept;
        
        // Views for iteration
        template <Component... Ts>
        auto view();  // Returns entities with all specified components
        
        template <Component... Ts>
        auto zipView();  // Returns ZipView for parallel iteration
        
    private:
        std::unordered_map<std::type_index, 
                           std::unique_ptr<ISparseArray>> _arrays;
        std::vector<std::uint32_t> _generations;
        std::deque<std::size_t> _freeIndices;
        mutable std::shared_mutex _mutex;
    };
}
```

### ZipView

`ZipView` enables efficient iteration over multiple component types:

```cpp
// common/include/RType/ECS/ZipView.hpp
template <typename... Arrays>
class ZipView {
public:
    // Iterator yields tuple of (Entity, Component&...)
    auto begin();
    auto end();
};
```

**Usage Example**:
```cpp
auto& registry = getRegistry();

// Register components
registry.subscribe<Transform>();
registry.subscribe<Velocity>();
registry.subscribe<Health>();

// Spawn entity with components
auto entity = registry.spawn().value();
registry.add<Transform>(entity, Vec2f{100.f, 200.f});
registry.add<Velocity>(entity, 5.0f, Vec2f{1.f, 0.f});
registry.add<Health>(entity, 100, 100);

// Iterate entities with Transform + Velocity
for (auto [e, transform, velocity] : registry.zipView<Transform, Velocity>()) {
    transform.position += velocity.direction * velocity.speed;
}
```

### Systems

Systems contain game logic. They are not enforced by the ECS framework but follow a pattern:

```cpp
// Example: server/src/Systems/MovementSystem.cpp
class MovementSystem {
public:
    void update(ecs::Registry& registry, float deltaTime) {
        for (auto [entity, transform, velocity] : 
             registry.zipView<Transform, Velocity>()) {
            transform.position.x += velocity.direction.x * velocity.speed * deltaTime;
            transform.position.y += velocity.direction.y * velocity.speed * deltaTime;
        }
    }
};
```

**Server Systems** (`server/src/Systems/`):
- `MovementSystem` - Entity movement
- `CollisionSystem` - Collision detection
- `EnemyAISystem` - Enemy behavior
- `EnemyShootSystem` - Enemy firing logic
- `PlayerShootSystem` - Player weapon handling
- `BulletCleanupSystem` - Remove off-screen projectiles
- `LevelSystem` - Level progression
- `NetworkSyncSystem` - Broadcast state to clients

**Client Systems** (`client/src/Systems/`):
- `RenderSystem` - Draw sprites
- `AnimationSystem` - Update animations
- `InputSystem` - Process player input
- `AudioSystem` - Sound playback
- `UISystem` - UI interactions
- `NetworkSyncSystem` - Process server packets

---

## Network Layer

The network layer uses **ASIO** for asynchronous TCP/UDP communication.

### Protocol

Located in `common/include/RType/Network/Packet.hpp`:

```cpp
namespace rtp::net
{
    constexpr uint16_t MAGIC_NUMBER = 0xA1B2;
    constexpr size_t MTU_SIZE = 1400;

    enum class OpCode : uint8_t {
        // Connection
        Hello = 0x01,
        Welcome = 0x02,
        Disconnect = 0x03,
        
        // Authentication
        LoginRequest = 0x1A,
        LoginResponse = 0x9A,
        
        // Lobby
        ListRooms = 0x04,
        CreateRoom = 0x06,
        JoinRoom = 0x07,
        StartGame = 0x0D,
        
        // Gameplay
        InputTick = 0x10,        // Client → Server
        EntitySpawn = 0x21,      // Server → Client
        EntityDeath = 0x22,      // Server → Client
        AmmoUpdate = 0x23,
        Ping = 0x24,
        Pong = 0x25,
        // ...
    };

    #pragma pack(push, 1)
    struct Header {
        uint16_t magic = MAGIC_NUMBER;
        uint16_t sequenceId = 0;
        uint32_t bodySize = 0;
        uint16_t ackId = 0;
        OpCode opCode = OpCode::None;
        uint8_t reserved = 0;
        uint32_t sessionId = 0;
    };
    #pragma pack(pop)
}
```

### Packet Class

```cpp
class Packet {
public:
    // Construction
    Packet();
    explicit Packet(OpCode opCode);
    
    // Serialization
    template <typename T>
    Packet& operator<<(const T& value);
    
    template <typename T>
    Packet& operator>>(T& value);
    
    // Access
    Header& header();
    std::span<uint8_t> body();
    std::vector<uint8_t> serialize() const;
    
    // Deserialization
    static Packet deserialize(std::span<const uint8_t> data);
};
```

### Network Modes

```cpp
enum class NetworkMode {
    TCP,  // Reliable, ordered (connection, chat, critical events)
    UDP   // Fast, unreliable (position updates, inputs)
};
```

### Client Network

```cpp
// client/include/Network/ClientNetwork.hpp
class ClientNetwork : public net::INetwork {
public:
    ClientNetwork(const std::string& serverIp, uint16_t serverPort);
    
    void start() override;
    void stop() override;
    
    void sendPacket(const net::Packet& packet, net::NetworkMode mode);
    std::optional<net::NetworkEvent> pollEvent() override;
};
```

### Server Network

```cpp
// server/include/ServerNetwork/ServerNetwork.hpp
class ServerNetwork : public net::INetwork {
public:
    explicit ServerNetwork(uint16_t port);
    
    void start() override;
    void stop() override;
    
    void sendPacket(uint32_t sessionId, const net::Packet& packet, 
                    net::NetworkMode mode);
    void broadcastPacket(const net::Packet& packet, net::NetworkMode mode);
    
    std::optional<net::NetworkEvent> pollEvent() override;
};
```

---

## Client/Server Separation

### Server Responsibilities

1. **Game Logic**: All physics, collision, AI, scoring
2. **State Management**: Authoritative game state
3. **Entity Lifecycle**: Spawn/kill entities
4. **Input Processing**: Validate and apply player inputs
5. **Broadcasting**: Send state updates to all clients

### Client Responsibilities

1. **Rendering**: Draw game state
2. **Audio**: Play sounds and music
3. **Input Capture**: Send inputs to server
4. **UI**: Menu, HUD, settings
5. **Local Prediction** (optional): Smooth rendering

### Data Flow

```
Client Input              Server Processing           Client Rendering
─────────────────────────────────────────────────────────────────────
[Player Press Space]
        │
        ▼
  InputTick Packet ───────► [Validate Input]
   (OpCode: 0x10)           [Create Bullet Entity]
                            [Add to Registry]
                                    │
                                    ▼
                            EntitySpawn Packet ─────► [Create Local Entity]
                             (OpCode: 0x21)          [Add Sprite Component]
                                                     [Render Next Frame]
```

---

## Building & Testing

### Debug Build

```bash
./scripts/build.sh debug
```

Creates `r-type_client_debug` and `r-type_server_debug` with debug symbols and debug key bindings.

### Running Tests

```bash
./scripts/build.sh test
```

### Code Coverage

```bash
./scripts/build.sh coverage
# Report in build/coverage.xml
```

### Test Structure

```
tests/
├── ecs/
│   ├── test_components.cpp
│   ├── test_registry.cpp
│   ├── test_sparsearray.cpp
│   └── test_zipview.cpp
├── logger/
│   └── test_logger.cpp
└── network/
    └── test_protocol.cpp
```

---

## Code Style

### Formatting

Use `.clang-format` configuration:

```bash
./scripts/format.sh
```

### Linting

Use `.clang-tidy` configuration:

```bash
./scripts/lint.sh
```

### Naming Conventions

| Element | Style | Example |
|---------|-------|---------|
| Classes | PascalCase | `SparseArray`, `ClientNetwork` |
| Functions | camelCase | `spawn()`, `pollEvent()` |
| Variables | camelCase | `entityCount`, `serverPort` |
| Constants | UPPER_SNAKE | `MAX_COMPONENTS`, `MTU_SIZE` |
| Private members | _prefix | `_arrays`, `_mutex` |
| Namespaces | lowercase | `rtp::ecs`, `rtp::net` |
| Files | PascalCase | `Registry.hpp`, `Packet.cpp` |

### Include Order

```cpp
// 1. Related header
#include "MyClass.hpp"

// 2. Project headers
#include "RType/ECS/Registry.hpp"
#include "RType/Network/Packet.hpp"

// 3. Third-party headers
#include <asio.hpp>
#include <SFML/Graphics.hpp>

// 4. Standard library
#include <vector>
#include <memory>
```

---

## Further Reading

- [Extension Guide](EXTENSION_GUIDE.md) - Adding new content
- [Architecture Deep Dive](ARCHITECTURE.md) - Detailed architecture
- [User Guide](USER_GUIDE.md) - End-user documentation
