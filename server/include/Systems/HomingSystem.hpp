/**
 * File   : HomingSystem.hpp
 * License: MIT
 */

#pragma once

#include "RType/ECS/ISystem.hpp"
#include "RType/ECS/Registry.hpp"

#include "RType/ECS/Components/Transform.hpp"
#include "RType/ECS/Components/Velocity.hpp"
#include "RType/ECS/Components/EntityType.hpp"
#include "RType/ECS/Components/RoomId.hpp"
#include "RType/ECS/Components/Health.hpp"
#include "RType/ECS/Components/Homing.hpp"

namespace rtp::server {

class HomingSystem : public ecs::ISystem {
  public:
    explicit HomingSystem(ecs::Registry &registry);
    void update(float dt) override;

  private:
    ecs::Registry &_registry;
};

} // namespace rtp::server
