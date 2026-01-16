/**
 * File   : UiFactory.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 */

#include "engine/ui/UiFactory.hpp"

namespace engine::ui
{
    //////////////////////////////////////////////////////////////////////////
    // Public API
    //////////////////////////////////////////////////////////////////////////

    engine::ecs::Entity spawnNewEntity(engine::ecs::Registry& registry)
    {
        auto entityRes = registry.spawn();
        if (!entityRes) {
            throw std::runtime_error(std::string("Failed to spawn entity: ") + std::string(entityRes.error().message()));
        }
        return entityRes.value();
    }

    engine::ecs::Entity UiFactory::createButton(
        engine::ecs::Registry& registry,
        const position& position,
        const size& size,
        const textContainer& textInfo,
        std::function<void()> onClick)
    {
        engine::ecs::Entity entity = spawnNewEntity(registry);

        engine::ecs::components::Button button;
        button.position = { position.x, position.y };
        button.size = { size.x, size.y };
        button.text = textInfo;
        button.size = { size.x, size.y };
        button.onClick = onClick;

        registry.add<engine::ecs::components::Button>(entity, button);

        return entity;
        
    };

    engine::ecs::Entity UiFactory::createText(
        engine::ecs::Registry& registry,
        const position& position,
        const textContainer& textInfo,
        const std::uint8_t zIndex,
        const color& textColor)
    {
        engine::ecs::Entity entity = spawnNewEntity(registry);

        engine::ecs::components::Text text;
        text.text = textInfo;
        text.position = { position.x, position.y };
        text.zIndex = zIndex;
        text.color = textColor;

        registry.add<engine::ecs::components::Text>(entity, text);

        return entity;

    };

    engine::ecs::Entity UiFactory::createSlider(
        engine::ecs::Registry& registry,
        const position& position,
        const size& size,
        float minValue,
        float maxValue,
        float initialValue,
        std::function<void(float)> onChange)
    {
        engine::ecs::Entity entity = spawnNewEntity(registry);

        engine::ecs::components::Slider slider;

        slider.position = position;
        slider.size = size;
        slider.minValue = minValue;
        slider.maxValue = maxValue;
        slider.currentValue = initialValue;
        slider.onChange = onChange;

        registry.add<engine::ecs::components::Slider>(entity, slider);

        return entity;

    };

    engine::ecs::Entity UiFactory::createDropdown(
        engine::ecs::Registry& registry,
        const position& position,
        const size& size,
        const std::vector<std::string>& options,
        const int selectedIndex,
        std::function<void(int)> onSelect)
    {
        engine::ecs::Entity entity = spawnNewEntity(registry);

        engine::ecs::components::Dropdown dropdown;

        dropdown.position = position;
        dropdown.size = size;
        dropdown.options = options;
        dropdown.selectedIndex = selectedIndex;
        dropdown.onSelect = onSelect;

        registry.add<engine::ecs::components::Dropdown>(entity, dropdown);

        return entity;
    };

    engine::ecs::Entity UiFactory::createTextInput(
        engine::ecs::Registry& registry,
        const position& position,
        const size& size,
        std::size_t maxLength,
        const textContainer& placeholder,
        std::function<void(const std::string&)> onSumbit,
        std::function<void(const std::string&)> onChange)
    {
        engine::ecs::Entity entity = spawnNewEntity(registry);

        engine::ecs::components::TextInput textInput;

        textInput.position = position;
        textInput.size = size;
        textInput.maxLength = maxLength;
        textInput.placeholder = placeholder;
        textInput.onSubmit = onSumbit;
        textInput.onChange = onChange;

        registry.add<engine::ecs::components::TextInput>(entity, textInput);

        return entity;
    };
} // namespace engine::ui