/**
 * File   : UiFactory.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#ifndef RTYPE_CLIENT_GRAPHICS_UIFACTORY_HPP_
#define RTYPE_CLIENT_GRAPHICS_UIFACTORY_HPP_

#include <memory>
#include <string>
#include <cstdint>

#include "RType/ECS/Components/UI/Button.hpp"
#include "RType/ECS/Components/UI/Text.hpp"
#include "RType/ECS/Components/UI/Slider.hpp"
#include "RType/ECS/Components/UI/Dropdown.hpp"
#include "RType/ECS/Components/UI/TextInput.hpp"
#include "RType/ECS/Components/UI/SpritePreview.hpp"
#include "RType/ECS/Registry.hpp"

namespace rtp::client {
    namespace graphics {

        /**
         * @struct color
         * @brief Represents an RGB color.
         */
        struct color {
            uint8_t r;
            uint8_t g;
            uint8_t b;
        };

        /**
         * @struct position
         * @brief Represents the position of a UI component.
         */
        struct position {
            float x;
            float y;
        };

        /**
         * @struct size
         * @brief Represents the size of a UI component.
         */
        struct size {
            float width;
            float height;
        };

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
                 * @param x X position of the button.
                 * @param y Y position of the button.
                 * @param width Width of the button.
                 * @param height Height of the button.
                 * @param label Text label of the button.
                 * @param onClick Callback function to be called when the button is clicked.
                 * @return The created entity representing the button.
                 */
                static rtp::ecs::Entity createButton(rtp::ecs::Registry& registry,
                                                    const position& position,
                                                    const size& size,
                                                    const std::string& label,
                                                    std::function<void()> onClick = nullptr);

                /**
                 * @brief Create a text UI component.
                 * @param registry Reference to the ECS registry.
                 * @param x X position of the text.
                 * @param y Y position of the text.
                 * @param content Text content.
                 * @param fontPath Path to the font file.
                 * @param fontSize Size of the font.
                 * @param zIndex Z-index for rendering order.
                 * @param textColor Color of the text.
                 * @return The created entity representing the text.
                 */
                static rtp::ecs::Entity createText(rtp::ecs::Registry& registry,
                                                const position& position,
                                                const std::string& content,
                                                const std::string& fontPath,
                                                unsigned int fontSize,
                                                const std::uint8_t zIndex = 0,
                                                const color& textColor = {255, 255, 255});

                /**
                 * @brief Create a slider UI component.
                 * @param registry Reference to the ECS registry.
                 * @param x X position of the slider.
                 * @param y Y position of the slider.
                 * @param width Width of the slider.
                 * @param height Height of the slider.
                 * @param minValue Minimum value of the slider.
                 * @param maxValue Maximum value of the slider.
                 * @param initialValue Initial value of the slider.
                 * @param onChange Callback function to be called when the slider value changes.
                 * @return The created entity representing the slider.
                 */
                static rtp::ecs::Entity createSlider(rtp::ecs::Registry& registry,
                                                    const position& position,
                                                    const size& size,
                                                    float minValue,
                                                    float maxValue,
                                                    float initialValue,
                                                    std::function<void(float)> onChange = nullptr
                                                    );

                /**
                 * @brief Create a dropdown UI component.
                 * @param registry Reference to the ECS registry.
                 * @param x X position of the dropdown.
                 * @param y Y position of the dropdown.
                 * @param width Width of the dropdown.
                 * @param height Height of the dropdown.
                 * @param options List of options for the dropdown.
                 * @param selectedIndex Index of the initially selected option.
                 * @param onSelect Callback function to be called when an option is selected.
                 * @return The created entity representing the dropdown.
                 */
                static rtp::ecs::Entity createDropdown(rtp::ecs::Registry& registry,
                                                    const position& position,
                                                    const size& size,
                                                    const std::vector<std::string>& options,
                                                    const int selectedIndex,
                                                    std::function<void(int index)> onSelect = nullptr
                                                    );

                /**
                 * @brief Create a text input UI component.
                 * @param registry Reference to the ECS registry.
                 * @param x X position of the text input.
                 * @param y Y position of the text input.
                 * @param width Width of the text input.
                 * @param height Height of the text input.
                 * @param fontPath Path to the font file.
                 * @param fontSize Size of the font.
                 * @param maxLength Maximum length of the input text.
                 * @param placeholder Placeholder text when input is empty.
                 * @param onChange Callback function to be called when the text changes.
                 * @return The created entity representing the text input.
                 */
                static rtp::ecs::Entity createTextInput(rtp::ecs::Registry& registry,
                                                        const position& position,
                                                        const size& size,
                                                        const std::string& fontPath,
                                                        unsigned int fontSize,
                                                        const int maxLength = 64,
                                                        const std::string& placeholder = "",
                                                        std::function<void(const std::string&)> onSubmit = nullptr,
                                                        std::function<void(const std::string&)> onChange = nullptr
                                                    );

                /**
                 * @brief Create a sprite preview UI component.
                 * @param registry Reference to the ECS registry.
                 * @param position Position of the sprite preview.
                 * @param texturePath Path to the texture file.
                 * @param rectLeft Left coordinate in the texture.
                 * @param rectTop Top coordinate in the texture.
                 * @param rectWidth Width of the sprite in the texture.
                 * @param rectHeight Height of the sprite in the texture.
                 * @param scale Scale factor for display.
                 * @param zIndex Z-index for rendering order.
                 * @return The created entity representing the sprite preview.
                 */
                static rtp::ecs::Entity createSpritePreview(rtp::ecs::Registry& registry,
                                                           const position& position,
                                                           const std::string& texturePath,
                                                           int rectLeft,
                                                           int rectTop,
                                                           int rectWidth,
                                                           int rectHeight,
                                                           float scale = 2.0f,
                                                           int zIndex = 0);
        };

    } // namespace graphics
} // namespace rtp::client

#endif // RTYPE_CLIENT_GRAPHICS_UIFACTORY_HPP_
