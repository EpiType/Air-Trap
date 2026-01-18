# 📦 Common Module

The common module contains shared code between the client and server, including the ECS engine, network protocol, and utility classes.

## Directory Structure

```
common/
├── include/RType/
│   ├── ECS/            # Entity-Component-System engine
│   │   ├── Components/ # All game components
│   │   ├── Entity.hpp
│   │   ├── Registry.hpp
│   │   ├── SparseArray.hpp
│   │   └── ZipView.hpp
│   ├── Network/        # Network protocol
│   ├── Math/           # Vector mathematics
│   ├── Config/         # Configuration loading
│   ├── Thread/         # Threading utilities
│   ├── Error.hpp       # Error handling
│   └── Logger.hpp      # Logging system
├── src/                # Implementation files
├── assets/             # Shared assets (levels, templates)
└── CMakeLists.txt      # Build configuration
```

## ECS Engine

### Entity

A 64-bit unique identifier with index and generation:

```cpp
class Entity {
public:
    constexpr std::uint32_t index() const;
    constexpr std::uint32_t generation() const;
    constexpr bool isNull() const noexcept;
};
```

### SparseArray

Efficient component storage with O(1) access:

```cpp
template <Component T>
class SparseArray {
public:
    T& emplace(Entity entity, Args&&... args);
    void erase(Entity entity) noexcept;
    bool has(Entity entity) const noexcept;
    auto&& operator[](Entity entity) noexcept;
    std::span<T> data() noexcept;
    std::span<Entity> entities() noexcept;
};
```

### Registry

Central entity and component manager:

```cpp
class Registry {
public:
    // Entity management
    auto spawn() -> std::expected<Entity, Error>;
    void kill(Entity entity);
    
    // Component operations
    template <Component T> auto subscribe();
    template <Component T> auto add(Entity entity, Args&&... args);
    template <Component T> bool has(Entity entity) const;
    template <Component T> void remove(Entity entity);
    
    // Views
    template <Component... Ts> auto view();
    template <Component... Ts> auto zipView();
};
```

### ZipView

Parallel iteration over multiple components:

```cpp
for (auto [entity, transform, velocity] : registry.zipView<Transform, Velocity>()) {
    transform.position += velocity.direction * velocity.speed;
}
```

## Components

Available components in `ECS/Components/`:

| Component | Purpose |
|-----------|---------|
| `Transform` | Position, rotation, scale |
| `Velocity` | Movement direction and speed |
| `Health` | Hit points |
| `Damage` | Damage dealt on collision |
| `Sprite` | Texture and rendering data |
| `Animation` | Frame animation |
| `Hitbox` | Collision bounds |
| `NetworkId` | Network synchronization ID |
| `Controllable` | Player-controlled flag |
| `IABehavior` | AI configuration |
| `ParallaxLayer` | Background parallax |
| `Ammo` | Weapon ammunition |
| `Shield` | Damage absorption |

## Network Protocol

### Packet Structure

```cpp
struct Header {
    uint16_t magic;      // 0xA1B2
    uint16_t sequenceId;
    uint32_t bodySize;
    uint16_t ackId;
    OpCode opCode;
    uint8_t reserved;
    uint32_t sessionId;
};
```

### OpCodes

| Range | Category |
|-------|----------|
| 0x01-0x03 | Connection |
| 0x04-0x0D | Lobby |
| 0x10-0x1F | Gameplay (C→S) |
| 0x1A-0x9B | Authentication |
| 0x20-0x2F | Game State (S→C) |

## Math

Vector types in `Math/`:

```cpp
template <typename T>
struct Vec2 {
    T x, y;
    
    Vec2 operator+(const Vec2& other) const;
    Vec2 operator*(T scalar) const;
    T dot(const Vec2& other) const;
    T length() const;
    Vec2 normalized() const;
};

using Vec2f = Vec2<float>;
using Vec2i = Vec2<int>;
```

## Logger

Thread-safe logging:

```cpp
LOG_INFO("Player {} connected", playerId);
LOG_DEBUG("Entity spawned at ({}, {})", x, y);
LOG_ERROR("Failed to load: {}", filename);
```

## Error Handling

Using `std::expected` for error propagation:

```cpp
auto result = registry.spawn();
if (!result) {
    LOG_ERROR("Spawn failed: {}", result.error().message());
    return;
}
Entity entity = result.value();
```

## Building

The common module is built as a static library:

```bash
cmake --build build --target RTypeCommon
```

Both client and server link against this library.
