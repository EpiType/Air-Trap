#pragma once

#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>
#include <typeindex>
#include <cstdint>

namespace events {

/**
 * @brief Event bus for decoupled communication between systems.
 * 
 * Implements the Mediator pattern to avoid direct system coupling.
 */
class EventBus {
public:
    EventBus() = default;
    ~EventBus() = default;

    /**
     * @brief Subscribe to an event type.
     * @tparam EventType The type of event to listen for
     * @param callback Function to call when event is published
     */
    template<typename EventType>
    void subscribe(std::function<void(const EventType&)> callback);

    /**
     * @brief Publish an event to all subscribers.
     * @tparam EventType The type of event
     * @param event The event data
     */
    template<typename EventType>
    void publish(const EventType& event);

private:
    std::unordered_map<std::type_index, std::vector<std::function<void(const void*)>>> _subscribers;
};

// Example events:
struct EntityCreatedEvent {
    std::uint32_t entityId;
};

struct EntityDestroyedEvent {
    std::uint32_t entityId;
};

struct PlayerInputEvent {
    std::uint32_t playerId;
    int inputFlags; // Bitmask for actions
};

} // namespace events
