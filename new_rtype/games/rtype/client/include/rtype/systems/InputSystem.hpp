/**
 * File   : InputSystem.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 16/01/2026
 */

#ifndef RTYPE_CLIENT_INPUT_SYSTEM_HPP_
    #define RTYPE_CLIENT_INPUT_SYSTEM_HPP_

    /* Engine */
    #include "engine/ecs/ISystem.hpp"
    #include "engine/ecs/Registry.hpp"
    #include "engine/input/Input.hpp"
    #include "engine/net/INetwork.hpp"
    #include "engine/ecs/components/ui/TextInput.hpp"

    /* R-Type */
    #include "rtype/utility/Settings.hpp"
    #include "rtype/Packet.hpp"

namespace rtp::client::systems
{
    /**
     * @class InputSystem
     * @brief System responsible for sending input ticks to the server.
     */
    class InputSystem : public engine::ecs::ISystem
    {
        public:
            /**
             * @brief Constructor for InputSystem
             * @param worldRegistry Reference to the world registry
             * @param uiRegistry Reference to the UI registry
             * @param settings Reference to client settings
             * @param network Reference to the client network
             * @param sessionId Session id to target
             */
            InputSystem(engine::ecs::Registry& worldRegistry,
                        engine::ecs::Registry& uiRegistry,
                        Settings& settings,
                        engine::net::INetwork& network,
                        uint32_t sessionId);

            /**
             * @brief Update input system logic for one frame.
             * @param dt Time elapsed since last update in seconds
             */
            void update(float) override;

            /**
             * @brief Update session id used for input packets.
             * @param sessionId New session id to target
             */
            void setSessionId(uint32_t sessionId);

        private:
            enum InputBits : uint8_t {              /**< Bitmask for input directions */
                MoveUp    = 1 << 0,
                MoveDown  = 1 << 1,
                MoveLeft  = 1 << 2,
                MoveRight = 1 << 3,
                Shoot     = 1 << 4,
                Reload    = 1 << 5
            };

            engine::ecs::Registry& _worldRegistry;  /**< Reference to the world registry */
            engine::ecs::Registry& _uiRegistry;     /**< Reference to the UI registry */
            Settings& _settings;                    /**< Reference to client settings */
            engine::net::INetwork& _network;        /**< Reference to the client network */
            uint32_t _sessionId{0};                 /**< Session id to target */
            uint8_t _lastMask{0};                   /**< Last sent input mask */
    };
}

#endif /* !RTYPE_CLIENT_INPUT_SYSTEM_HPP_ */
