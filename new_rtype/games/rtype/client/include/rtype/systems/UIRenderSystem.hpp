/**
 * File   : UIRenderSystem.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 16/01/2026
 */

#ifndef RTYPE_CLIENT_UI_RENDER_SYSTEM_HPP_
    #define RTYPE_CLIENT_UI_RENDER_SYSTEM_HPP_

    /* Engine */
    #include "engine/ecs/ISystem.hpp"
    #include "engine/ecs/Registry.hpp"
    #include "engine/render/IRenderer.hpp"
    #include "engine/render/RenderFrame.hpp"
    #include "engine/ecs/components/ui/Button.hpp"
    #include "engine/ecs/components/ui/Text.hpp"
    #include "engine/ecs/components/ui/Slider.hpp"
    #include "engine/ecs/components/ui/Dropdown.hpp"
    #include "engine/ecs/components/ui/TextInput.hpp"

    #include <string>
    #include <unordered_map>

namespace rtp::client::systems
{
    /**
     * @class UIRenderSystem
     * @brief System to build render frames for UI elements.
     */
    class UIRenderSystem : public aer::ecs::ISystem
    {
        public:
            /**
             * @brief Constructor for UIRenderSystem
             * @param registry Reference to the UI registry
             * @param renderer Reference to the renderer
             * @param frame Render frame to fill
             */
            UIRenderSystem(aer::ecs::Registry& registry,
                           aer::render::IRenderer& renderer,
                           aer::render::RenderFrame& frame);

            /**
             * @brief Build render frame for UI components.
             * @param dt Time elapsed since last update in seconds
             */
            void update(float dt) override;

        private:
            /**
             * @brief Render all button components.
             */
            void renderButtons();

            /**
             * @brief Render all text components.
             */
            void renderTexts();

            /**
             * @brief Render all slider components.
             */
            void renderSliders();

            /**
             * @brief Render all dropdown components.
             */
            void renderDropdowns();

            /**
             * @brief Render all text input components.
             * @param dt Time elapsed since last update in seconds
             */
            void renderTextInputs(float dt);

            /**
             * @brief Get or load font ID for the given font path.
             * @param path File path to the font
             * @return Unique identifier for the font
             */
            std::uint32_t getFontId(const std::string& path);

        private:
            aer::ecs::Registry& _registry;   /**< Reference to the UI registry */
            aer::render::IRenderer& _renderer; /**< Reference to the renderer */
            aer::render::RenderFrame& _frame;  /**< Render frame to fill */
            std::unordered_map<std::string, std::uint32_t> _fontCache; /**< Font cache */
    };
}

#endif /* !RTYPE_CLIENT_UI_RENDER_SYSTEM_HPP_ */
