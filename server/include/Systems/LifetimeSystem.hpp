/**
 * File   : LifetimeSystem.hpp
 * License: MIT
 * Author : GitHub Copilot
 * Date   : 18/01/2026
 */

#ifndef RTYPE_LIFETIME_SYSTEM_HPP_
    #define RTYPE_LIFETIME_SYSTEM_HPP_

    #include "RType/ECS/ISystem.hpp"
    #include "RType/ECS/Registry.hpp"
    #include "RType/ECS/Components/Lifetime.hpp"
    #include "RType/ECS/Components/NetworkId.hpp"
    #include "RType/ECS/Components/RoomId.hpp"

    #include "Systems/NetworkSyncSystem.hpp"
    #include "Systems/RoomSystem.hpp"

namespace rtp::server {
    /**
     * @class LifetimeSystem
     * @brief Manages entities with limited lifetime and despawns them when expired
     */
    class LifetimeSystem : public ecs::ISystem {
        public:
            /**
             * @brief Constructor for LifetimeSystem
             * @param registry Reference to the entity registry
             * @param networkSync Reference to NetworkSyncSystem for entity despawn packets
             * @param roomSystem Reference to RoomSystem
             */
            LifetimeSystem(ecs::Registry& registry,
                          NetworkSyncSystem& networkSync,
                          RoomSystem& roomSystem);

            /**
             * @brief Update lifetime system logic for one frame
             * @param dt Time elapsed since last update in seconds
             */
            void update(float dt) override;

        private:
            ecs::Registry& _registry;
            NetworkSyncSystem& _networkSync;
            RoomSystem& _roomSystem;
    };
}

#endif /* !RTYPE_LIFETIME_SYSTEM_HPP_ */
