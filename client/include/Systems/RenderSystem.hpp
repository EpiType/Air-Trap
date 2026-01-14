/**
 * File   : RenderSystem.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 14/12/2025
 */

#ifndef RTYPE_RENDER_SYSTEM_HPP_
    #define RTYPE_RENDER_SYSTEM_HPP_

    #include "RType/ECS/Registry.hpp"
    #include "RType/Logger.hpp"
    #include "RType/ECS/ISystem.hpp"
    
    #include <SFML/Graphics.hpp>
    #include <SFML/System.hpp>
    #include <unordered_map>
    #include <vector>
    #include <algorithm>
    #include <string>

    /* ECS Components */
    #include "RType/ECS/Components/Transform.hpp"
    #include "RType/ECS/Components/Sprite.hpp"
    #include "RType/ECS/Components/BoundingBox.hpp"
    #include "RType/ECS/Components/EntityType.hpp"

namespace rtp::client {

    class ModManager;

    class RenderSystem : public rtp::ecs::ISystem {
        public:
            RenderSystem(rtp::ecs::Registry& r, sf::RenderWindow& window) 
                : _r(r), _window(window), _modManager(nullptr) {}

            void setModManager(ModManager* modManager) { _modManager = modManager; }

        void update(float dt) override;

    private:
        rtp::ecs::Registry& _r;
        sf::RenderWindow& _window;
        ModManager* _modManager;
        
        std::unordered_map<std::string, sf::Texture> _textureCache;
    };
} // namespace rtp::client

#endif /* !RTYPE_RENDER_SYSTEM_HPP_ */
