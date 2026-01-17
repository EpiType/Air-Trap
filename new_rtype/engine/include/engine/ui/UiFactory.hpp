/**
 * File   : UiFactory.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 */

#ifndef ENGINE_GRAPHICS_UIFACTORY_HPP_
    #define ENGINE_GRAPHICS_UIFACTORY_HPP_

    #include "engine/ecs/Registry.hpp"
    #include "engine/math/Vec2.hpp"
    #include "engine/ui/UiUtilities.hpp"

    /* Components UI */
    #include "engine/ecs/components/ui/Button.hpp"
    #include "engine/ecs/components/ui/Text.hpp"
    #include "engine/ecs/components/ui/Slider.hpp"
    #include "engine/ecs/components/ui/Dropdown.hpp"
    #include "engine/ecs/components/ui/TextInput.hpp"

namespace engine::ui
{
    /**
     * @class UiFactory
     * @brief Factory class for creating UI components in the ECS registry.
     * 
     * This class provides static methods to create various UI elements
     * such as buttons, text fields, sliders, etc., and add them to the
     * provided ECS registry.
     */
    class UiFactory {
        public:
            UiFactory() = default;

            ~UiFactory() = default;

            /**
             * @brief Create a button UI component.
             * @param registry Reference to the ECS registry.
             * @param position Position of the button.
             * @param width Width of the button.
             * @param height Height of the button.
             * @param textInfo Text label of the button.
             * @param onClick Callback function to be called when the button is clicked.
             * @return The created entity representing the button.
             */
            static engine::ecs::Entity createButton(engine::ecs::Registry& registry,
                                                const position& position,
                                                const size& size,
                                                const textContainer& textInfo,
                                                std::function<void()> onClick = nullptr);

            /**
             * @brief Create a text UI component.
             * @param registry Reference to the ECS registry.
             * @param position Position of the text.
             * @param textInfo Text content, font path, and size.
             * @param zIndex Z-index for rendering order.
             * @param textColor Color of the text.
             * @return The created entity representing the text.
             */
            static engine::ecs::Entity createText(engine::ecs::Registry& registry,
                                                const position& position,
                                                const textContainer& textInfo,
                                                const std::uint8_t zIndex = 0,
                                                const color& textColor = {255, 255, 255});

            /**
             * @brief Create a slider UI component.
             * @param registry Reference to the ECS registry.
             * @param position Position of the slider.
             * @param width Width of the slider.
             * @param height Height of the slider.
             * @param minValue Minimum value of the slider.
             * @param maxValue Maximum value of the slider.
             * @param initialValue Initial value of the slider.
             * @param onChange Callback function to be called when the slider value changes.
             * @return The created entity representing the slider.
             */
            static engine::ecs::Entity createSlider(engine::ecs::Registry& registry,
                                                const position& position,
                                                const size& size,
                                                float minValue,
                                                float maxValue,
                                                float initialValue,
                                                std::function<void(float)> onChange = nullptr);

            /**
             * @brief Create a dropdown UI component.
             * @param registry Reference to the ECS registry.
             * @param position Position of the dropdown.
             * @param width Width of the dropdown.
             * @param height Height of the dropdown.
             * @param options List of options for the dropdown.
             * @param selectedIndex Index of the initially selected option.
             * @param onSelect Callback function to be called when an option is selected.
             * @return The created entity representing the dropdown.
             */
            static engine::ecs::Entity createDropdown(engine::ecs::Registry& registry,
                                                    const position& position,
                                                    const size& size,
                                                    const std::vector<std::string>& options,
                                                    const int selectedIndex = 0,
                                                    std::function<void(int)> onSelect = nullptr);

            /**
             * @brief Create a text input UI component.
             * @param registry Reference to the ECS registry.
             * @param position Position of the text input.
             * @param size Size of the text input.
             * @param placeholder Placeholder text for the input field.
             * @param maxLength Maximum length of the input text.
             * @param onTextChange Callback function to be called when the text changes.
             * @return The created entity representing the text input.
             */
            static engine::ecs::Entity createTextInput(engine::ecs::Registry& registry,
                                                    const position& position,
                                                    const size& size,
                                                    std::size_t maxLength = 64,
                                                    const textContainer& placeholder = {},
                                                    std::function<void(const std::string&)> onSumbit = nullptr,
                                                    std::function<void(const std::string&)> onChange = nullptr);
    }; // class UiFactory
}; // namespace engine::ui

#endif /* !ENGINE_GRAPHICS_UIFACTORY_HPP_ */
