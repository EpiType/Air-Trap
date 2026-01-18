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
    class UISystem : public aer::ecs::ISystem
    {
        public:
            /**
             * @brief Constructor for UISystem
             * @param registry Reference to the UI registry
             * @param settings Reference to client settings
             */
            UISystem(aer::ecs::Registry& registry,
                     aer::render::IRenderer& renderer,
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
            void handleEvent(const aer::input::Event& event);

            /**
             * @brief Render the gamepad cursor on the screen.
             * @param renderer Reference to the renderer
             */
            void renderGamepadCursor(aer::render::IRenderer& renderer);

        private:
            void handleMouseMove(aer::math::Vec2f mousePos);
            void handleMouseClick(aer::math::Vec2f mousePos);

            bool isMouseOverButton(const aer::ecs::components::Button& button,
                                 const aer::math::Vec2f& mousePos);

            bool isMouseOverSlider(const aer::ecs::components::Slider& slider,
                                 const aer::math::Vec2f& mousePos);

            bool isMouseOverDropdown(const aer::ecs::components::Dropdown& dropdown,
                                     const aer::math::Vec2f& mousePos);

            bool updateSliderValue(aer::ecs::components::Slider& slider,
                                 const aer::math::Vec2f& mousePos);

            int getDropdownOptionAtMouse(const aer::ecs::components::Dropdown& dropdown,
                                         const aer::math::Vec2f& mousePos);

            bool isMouseOverTextInput(const aer::ecs::components::TextInput& input,
                                     const aer::math::Vec2f& mousePos) const;

            void focusTextInputAt(const aer::math::Vec2f& mousePos);
            void clearAllTextInputFocus();

            void handleTextInput(std::uint32_t unicode);

            void handleKeyPressed(aer::input::KeyCode key);

            void updateGamepadCursor(float dt);

            void handleGamepadInput();

        private:
            aer::ecs::Registry& _registry;
            aer::render::IRenderer& _renderer;
            Settings& _settings;

            aer::math::Vec2f _gamepadCursorPos;
            bool _gamepadMode{false};
    };
}

#endif /* !RTYPE_CLIENT_UI_SYSTEM_HPP_ */
