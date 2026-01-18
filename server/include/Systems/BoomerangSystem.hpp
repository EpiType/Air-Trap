/**
 * File   : BoomerangSystem.hpp
 * License: MIT
 */

#pragma once

#include "RType/ECS/ISystem.hpp"
#include "RType/ECS/Registry.hpp"

#include "RType/ECS/Components/Transform.hpp"
#include "RType/ECS/Components/Velocity.hpp"
#include "RType/ECS/Components/Boomerang.hpp"
#include "RType/ECS/Components/RoomId.hpp"

namespace rtp::server {

class BoomerangSystem : public ecs::ISystem {
  public:
    explicit BoomerangSystem(ecs::Registry &registry);
    void update(float dt) override;

  private:
    ecs::Registry &_registry;
};

} // namespace rtp::server
