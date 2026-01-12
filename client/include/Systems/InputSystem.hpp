/**
 * File   : InputSystem.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 14/12/2025
 */

#ifndef INPUT_SYSTEM_HPP
#define INPUT_SYSTEM_HPP

#include "RType/ECS/ISystem.hpp"
#include "RType/ECS/Registry.hpp"
#include "Core/Settings.hpp"
#include "Network/ClientNetwork.hpp"
#include "RType/Network/Packet.hpp"
#include <SFML/Graphics/RenderWindow.hpp>

#include <SFML/Window/Keyboard.hpp>
#include <cstdint>

namespace rtp::client {

class InputSystem : public rtp::ecs::ISystem
{
    public:
        /** 
         * @brief Constructor for InputSystem
         * @param r Reference to the entity registry
         * @param settings Reference to the client settings
         * @param net Reference to the client network manager
         * @param window Reference to the SFML render window
         */
        explicit InputSystem(rtp::ecs::Registry& r,
                            Settings& settings,
                            ClientNetwork& net,
                            sf::RenderWindow& window);
        
        /**
         * @brief Update input system logic for one frame
         * @param deltaTime Time elapsed since last update in seconds
         */
        void update(float) override;

    private:
        enum InputBits : uint8_t {              /**< Bitmask for input directions */
            MoveUp    = 1 << 0,
            MoveDown  = 1 << 1,
            MoveLeft  = 1 << 2,
            MoveRight = 1 << 3
        };

        rtp::ecs::Registry& _r;                 /**< Reference to the entity registry */
        Settings& _settings;                    /**< Reference to the client settings */
        ClientNetwork& _net;                    /**< Reference to the client network manager */
        sf::RenderWindow& _window;              /**< Reference to the SFML render window */

        uint8_t _lastMask = 0;                  /**< Last sent input mask */
    };

} // namespace rtp::client

#endif
