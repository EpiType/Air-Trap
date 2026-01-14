/**
 * File   : UiFactory.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#include "UI/UiFactory.hpp"

namespace rtp::client {
    namespace graphics {

        //////////////////////////////////////////////////////////////////////////
        // Public API
        //////////////////////////////////////////////////////////////////////////

        ecs::Entity UiFactory::createButton(
            ecs::Registry& registry,
            const position& position,
            const size& size,
            const std::string& label,
            std::function<void()> onClick)
        {
            auto entityRes = registry.spawn();
            if (!entityRes) {
                log::error("Failed to spawn button entity: {}", entityRes.error().message());
                throw std::runtime_error(std::string("Failed to spawn button entity: ") + std::string(entityRes.error().message()));
            }

            ecs::Entity entity = entityRes.value();

            ecs::components::ui::Button button;
            button.position = { position.x, position.y };
            button.size = { size.width, size.height };
            button.text = label;
            button.onClick = onClick;

            registry.add<ecs::components::ui::Button>(entity, button);

            return entity;
        }

        ecs::Entity UiFactory::createText(
            ecs::Registry& registry,
            const position& position,
            const std::string& content,
            const std::string& fontPath,
            unsigned int fontSize,
            const std::uint8_t zIndex,
            const color& textColor)
        {
            auto entityRes = registry.spawn();
            if (!entityRes) {
                log::error("Failed to spawn text entity: {}", entityRes.error().message());
                throw std::runtime_error(std::string("Failed to spawn text entity: ") + std::string(entityRes.error().message()));
            }

            ecs::Entity entity = entityRes.value();

            ecs::components::ui::Text text;
            text.position = { position.x, position.y };
            text.content = content;
            text.fontPath = fontPath;
            text.fontSize = fontSize;
            text.zIndex = zIndex;
            text.red = textColor.r;
            text.green = textColor.g;
            text.blue = textColor.b;

            registry.add<ecs::components::ui::Text>(entity, text);

            return entity;
        }

        ecs::Entity UiFactory::createSlider(
            ecs::Registry& registry,
            const position& position,
            const size& size,
            float minValue,
            float maxValue,
            float initialValue,
            std::function<void(float)> onChange)
        {
            auto entityRes = registry.spawn();
            if (!entityRes) {
                log::error("Failed to spawn slider entity: {}", entityRes.error().message());
                throw std::runtime_error(std::string("Failed to spawn slider entity: ") + std::string(entityRes.error().message()));
            }

            ecs::Entity entity = entityRes.value();

            ecs::components::ui::Slider slider;

            slider.position = { position.x, position.y };
            slider.size = { size.width, size.height };
            slider.minValue = minValue;
            slider.maxValue = maxValue;
            slider.currentValue = initialValue;
            slider.onChange = onChange;

            registry.add<ecs::components::ui::Slider>(entity, slider);

            return entity;
        }

        ecs::Entity UiFactory::createDropdown(
            ecs::Registry& registry,
            const position& position,
            const size& size,
            const std::vector<std::string>& options,
            const int selectedIndex,
            std::function<void(int index)> onSelect)
        {
            auto entityRes = registry.spawn();
            if (!entityRes) {
                log::error("Failed to spawn dropdown entity: {}", entityRes.error().message());
                throw std::runtime_error(std::string("Failed to spawn dropdown entity: ") + std::string(entityRes.error().message()));
            }

            ecs::Entity entity = entityRes.value();

            ecs::components::ui::Dropdown dropdown;

            dropdown.position = { position.x, position.y - 5.0f };
            dropdown.size = { size.width, size.height };
            dropdown.options = options;
            dropdown.selectedIndex = selectedIndex;
            dropdown.onSelect = [options, onSelect](int index) {
                if (index < 0 || index >= static_cast<int>(options.size())) {
                    log::warning("Dropdown selection index {} is out of bounds", index);
                    return;
                }
                if (onSelect) {
                    onSelect(index);
                }
            };

            registry.add<ecs::components::ui::Dropdown>(entity, dropdown);
            return entity;
        }

        ecs::Entity UiFactory::createTextInput(
            ecs::Registry& registry,
            const position& position,
            const size& size,
            const std::string& fontPath,
            unsigned int fontSize,
            const int maxLength,
            const std::string& placeholder,
            std::function<void(const std::string&)> onSubmit,
            std::function<void(const std::string&)> onChange)
        {
            auto entityRes = registry.spawn();
            if (!entityRes) {
                log::error("Failed to spawn text input entity: {}", entityRes.error().message());
                throw std::runtime_error(std::string("Failed to spawn text input entity: ") + std::string(entityRes.error().message()));
            }

            ecs::Entity entity = entityRes.value();

            ecs::components::ui::TextInput textInput;

            textInput.position = { position.x, position.y };
            textInput.size = { size.width, size.height };
            textInput.fontPath = fontPath;
            textInput.fontSize = fontSize;
            textInput.maxLength = maxLength;
            textInput.placeholder = placeholder;
            textInput.onSubmit = onSubmit;
            textInput.onChange = onChange;

            registry.add<ecs::components::ui::TextInput>(entity, textInput);

            return entity;
        }
    } // namespace graphics
} // namespace rtp::client