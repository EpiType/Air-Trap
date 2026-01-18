/**
 * File   : RenderSystem.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 16/01/2026
 */

#ifndef RTYPE_CLIENT_RENDER_SYSTEM_HPP_
    #define RTYPE_CLIENT_RENDER_SYSTEM_HPP_

    /* Engine */
    #include "engine/ecs/ISystem.hpp"
    #include "engine/ecs/Registry.hpp"
    #include "engine/ecs/components/Transform.hpp"
    #include "engine/ecs/components/Sprite.hpp"
    #include "engine/render/IRenderer.hpp"
    #include "engine/render/RenderFrame.hpp"

    #include <string>
    #include <unordered_map>

namespace rtp::client::systems
{
    /**
     * @class RenderSystem
     * @brief System to build render frames for world entities.
     */
    class RenderSystem : public aer::ecs::ISystem
    {
        public:
            /**
             * @brief Constructor for RenderSystem
             * @param registry Reference to the entity registry
             * @param renderer Reference to the renderer
             * @param frame Render frame to fill
             */
            RenderSystem(aer::ecs::Registry& registry,
                         aer::render::IRenderer& renderer,
                         aer::render::RenderFrame& frame);

            /**
             * @brief Build render frame for sprites.
             * @param dt Time elapsed since last update in seconds
             */
            void update(float dt) override;

        private:
            /**
             * @brief Get or load texture ID for the given texture path.
             * @param path File path to the texture
             * @return Unique identifier for the texture
             */
            std::uint32_t getTextureId(const std::string& path);

        private:
            aer::ecs::Registry& _registry;               /**< Reference to the entity registry */
            aer::render::IRenderer& _renderer;           /**< Reference to the renderer */
            aer::render::RenderFrame& _frame;            /**< Render frame to fill */
            std::unordered_map<std::string,
                std::uint32_t> _textureCache;               /**< Texture cache */
    };
}

#endif /* !RTYPE_CLIENT_RENDER_SYSTEM_HPP_ */
