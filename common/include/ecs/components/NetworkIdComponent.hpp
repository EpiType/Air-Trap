#pragma once

#include <cstdint>

namespace ecs {

struct NetworkIdComponent {
    std::uint32_t networkId;
    bool isOwned; // True if this client owns this entity
};

} // namespace ecs
