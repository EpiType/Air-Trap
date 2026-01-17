/**
 * File   : UISystem.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 16/01/2026
 */

#ifndef RTYPE_CLIENT_UI_SYSTEM_HPP_
    #define RTYPE_CLIENT_UI_SYSTEM_HPP_

    /* Engine */
    #include "engine/ecs/ISystem.hpp"
    #include "engine/ecs/Registry.hpp"
    #include "engine/input/Input.hpp"
    #include "engine/render/IRenderer.hpp"
    #include "engine/ecs/components/ui/Button.hpp"
    #include "engine/ecs/components/ui/Slider.hpp"
    #include "engine/ecs/components/ui/Dropdown.hpp"
    #include "engine/ecs/components/ui/TextInput.hpp"

    /* R-Type */
    #include "rtype/utility/Settings.hpp"

namespace rtp::client::systems
{
    /**
     * @class UISystem
     * @brief System to handle UI interactions.
     */
    class UISystem : public engine::ecs::ISystem
    {
        public:
            /**
             * @brief Constructor for UISystem
             * @param registry Reference to the UI registry
             * @param settings Reference to client settings
             */
            UISystem(engine::ecs::Registry& registry,
                     engine::render::IRenderer& renderer,
                     Settings& settings);

            /**
             * @brief Update UI state using current input.
             * @param dt Time elapsed since last update in seconds
             */
            void update(float dt) override;

            /**
             * @brief Handle input event for UI interactions.
             * @param event Input event to process
             */
            void handleEvent(const engine::input::Event& event);

            /**
             * @brief Render the gamepad cursor on the screen.
             * @param renderer Reference to the renderer
             */
            void renderGamepadCursor(engine::render::IRenderer& renderer);

        private:
            void handleMouseMove(engine::math::Vec2f mousePos);
            void handleMouseClick(engine::math::Vec2f mousePos);

            bool isMouseOverButton(const engine::ecs::components::Button& button,
                                 const engine::math::Vec2f& mousePos);

            bool isMouseOverSlider(const engine::ecs::components::Slider& slider,
                                 const engine::math::Vec2f& mousePos);

            bool isMouseOverDropdown(const engine::ecs::components::Dropdown& dropdown,
                                     const engine::math::Vec2f& mousePos);

            bool updateSliderValue(engine::ecs::components::Slider& slider,
                                 const engine::math::Vec2f& mousePos);

            int getDropdownOptionAtMouse(const engine::ecs::components::Dropdown& dropdown,
                                         const engine::math::Vec2f& mousePos);

            bool isMouseOverTextInput(const engine::ecs::components::TextInput& input,
                                     const engine::math::Vec2f& mousePos) const;

            void focusTextInputAt(const engine::math::Vec2f& mousePos);
            void clearAllTextInputFocus();

            void handleTextInput(std::uint32_t unicode);

            void handleKeyPressed(engine::input::KeyCode key);

            void updateGamepadCursor(float dt);

            void handleGamepadInput();

        private:
            engine::ecs::Registry& _registry;
            engine::render::IRenderer& _renderer;
            Settings& _settings;

            engine::math::Vec2f _gamepadCursorPos;
            bool _gamepadMode{false};
    };
}

#endif /* !RTYPE_CLIENT_UI_SYSTEM_HPP_ */
