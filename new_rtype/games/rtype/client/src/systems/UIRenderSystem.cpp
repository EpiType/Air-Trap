/**
 * File   : UIRenderSystem.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 16/01/2026
 */

#include "rtype/systems/UIRenderSystem.hpp"

#include <algorithm>

namespace rtp::client::systems
{
    namespace
    {
        float toUnit(uint8_t value)
        {
            return static_cast<float>(value) / 255.0f;
        }

        void applyColor(aer::render::RenderRect &shape, const aer::ui::color &color)
        {
            shape.r = toUnit(color.r);
            shape.g = toUnit(color.g);
            shape.b = toUnit(color.b);
            shape.a = toUnit(color.a.value_or(255));
        }

        void applyColor(aer::render::RenderText &text, const aer::ui::color &color)
        {
            text.r = toUnit(color.r);
            text.g = toUnit(color.g);
            text.b = toUnit(color.b);
            text.a = toUnit(color.a.value_or(255));
        }
    }

    //////////////////////////////////////////////////////////////////////////
    // Public API
    //////////////////////////////////////////////////////////////////////////

    UIRenderSystem::UIRenderSystem(aer::ecs::Registry& registry,
                                   aer::render::IRenderer& renderer,
                                   aer::render::RenderFrame& frame)
        : _registry(registry), _renderer(renderer), _frame(frame)
    {
    }

    void UIRenderSystem::update(float dt)
    {
        _frame.texts.clear();
        _frame.shapes.clear();

        renderButtons();
        renderTexts();
        renderSliders();
        renderDropdowns();
        renderTextInputs(dt);
    }

    //////////////////////////////////////////////////////////////////////////
    // Private API
    //////////////////////////////////////////////////////////////////////////

    void UIRenderSystem::renderButtons()
    {
        auto buttonsOpt = _registry.getComponents<aer::ecs::components::Button>();
        if (!buttonsOpt) {
            return;
        }
        auto &buttons = buttonsOpt.value().get();

        for (const auto &e : buttons.entities()) {
            const auto &button = buttons[e];

            aer::render::RenderRect rect;
            rect.position = { button.position.x, button.position.y };
            rect.size = { button.size.x, button.size.y };
            rect.filled = true;
            rect.z = 10;

            switch (button.state) {
                case aer::ecs::components::ButtonState::Pressed:
                    applyColor(rect, button.pressedColor);
                    break;
                case aer::ecs::components::ButtonState::Hovered:
                    applyColor(rect, button.hoverColor);
                    break;
                default:
                    applyColor(rect, button.idleColor);
                    break;
            }

            _frame.shapes.push_back(rect);

            aer::render::RenderText text;
            text.content = button.text.content;
            text.fontId = getFontId(button.text.FontPath);
            text.size = static_cast<int>(button.text.fontSize);
            text.position = { button.position.x + 12.0f, button.position.y + 6.0f };
            text.z = 11;
            applyColor(text, aer::ui::color{255, 255, 255, 255});
            _frame.texts.push_back(text);
        }
    }

    void UIRenderSystem::renderTexts()
    {
        auto textsOpt = _registry.getComponents<aer::ecs::components::Text>();
        if (!textsOpt) {
            return;
        }
        auto &texts = textsOpt.value().get();

        for (const auto &e : texts.entities()) {
            const auto &textComp = texts[e];

            aer::render::RenderText text;
            text.content = textComp.text.content;
            text.fontId = getFontId(textComp.text.FontPath);
            text.size = static_cast<int>(textComp.text.fontSize);
            text.position = { textComp.position.x, textComp.position.y };
            text.z = textComp.zIndex;
            applyColor(text, textComp.color);
            _frame.texts.push_back(text);
        }
    }

    void UIRenderSystem::renderSliders()
    {
        auto slidersOpt = _registry.getComponents<aer::ecs::components::Slider>();
        if (!slidersOpt) {
            return;
        }
        auto &sliders = slidersOpt.value().get();

        for (const auto &e : sliders.entities()) {
            const auto &slider = sliders[e];

            aer::render::RenderRect track;
            track.position = { slider.position.x, slider.position.y };
            track.size = { slider.size.x, slider.size.y };
            track.filled = true;
            track.z = slider.zIndex;
            applyColor(track, slider.trackColor);
            _frame.shapes.push_back(track);

            aer::render::RenderRect fill;
            const float filledWidth = slider.size.x * slider.getNormalized();
            fill.position = { slider.position.x, slider.position.y };
            fill.size = { filledWidth, slider.size.y };
            fill.filled = true;
            fill.z = slider.zIndex + 1;
            applyColor(fill, slider.fillColor);
            _frame.shapes.push_back(fill);

            aer::render::RenderRect handle;
            handle.position = { slider.position.x + filledWidth - 5.0f, slider.position.y - 2.0f };
            handle.size = { 10.0f, slider.size.y + 4.0f };
            handle.filled = true;
            handle.z = slider.zIndex + 2;
            applyColor(handle, slider.handleColor);
            _frame.shapes.push_back(handle);
        }
    }

    void UIRenderSystem::renderDropdowns()
    {
        auto dropdownsOpt = _registry.getComponents<aer::ecs::components::Dropdown>();
        if (!dropdownsOpt) {
            return;
        }
        auto &dropdowns = dropdownsOpt.value().get();

        for (const auto &e : dropdowns.entities()) {
            const auto &dropdown = dropdowns[e];

            aer::render::RenderRect base;
            base.position = { dropdown.position.x, dropdown.position.y };
            base.size = { dropdown.size.x, dropdown.size.y };
            base.filled = true;
            base.z = dropdown.zIndex;
            applyColor(base, dropdown.bgColor);
            _frame.shapes.push_back(base);

            aer::render::RenderText label;
            label.content = dropdown.getSelected();
            label.fontId = getFontId("assets/fonts/main.ttf");
            label.size = 16;
            label.position = { dropdown.position.x + 12.0f, dropdown.position.y + 6.0f };
            label.z = dropdown.zIndex + 1;
            applyColor(label, dropdown.textColor);
            _frame.texts.push_back(label);

            if (!dropdown.isOpen) {
                continue;
            }

            for (std::size_t i = 0; i < dropdown.options.size(); ++i) {
                aer::render::RenderRect opt;
                opt.position = { dropdown.position.x,
                                 dropdown.position.y + dropdown.size.y * static_cast<float>(i + 1) };
                opt.size = { dropdown.size.x, dropdown.size.y };
                opt.filled = true;
                opt.z = dropdown.zIndex + 2;
                applyColor(opt, dropdown.hoveredIndex == static_cast<int>(i) ?
                                   dropdown.hoverColor : dropdown.bgColor);
                _frame.shapes.push_back(opt);

                aer::render::RenderText optText;
                optText.content = dropdown.options[i];
                optText.fontId = label.fontId;
                optText.size = 16;
                optText.position = { opt.position.x + 12.0f, opt.position.y + 6.0f };
                optText.z = dropdown.zIndex + 3;
                applyColor(optText, dropdown.textColor);
                _frame.texts.push_back(optText);
            }
        }
    }

    void UIRenderSystem::renderTextInputs(float dt)
    {
        (void)dt;
        auto inputsOpt = _registry.getComponents<aer::ecs::components::TextInput>();
        if (!inputsOpt) {
            return;
        }
        auto &inputs = inputsOpt.value().get();

        for (const auto &e : inputs.entities()) {
            const auto &input = inputs[e];

            aer::render::RenderRect background;
            background.position = { input.position.x, input.position.y };
            background.size = { input.size.x, input.size.y };
            background.filled = true;
            background.z = input.zIndex;
            applyColor(background, input.bgColor);
            _frame.shapes.push_back(background);

            aer::render::RenderRect border;
            border.position = { input.position.x, input.position.y };
            border.size = { input.size.x, input.size.y };
            border.filled = false;
            border.thickness = 2.0f;
            border.z = input.zIndex + 1;
            applyColor(border, input.isFocused ? input.focusBorderColor : input.borderColor);
            _frame.shapes.push_back(border);

            aer::render::RenderText text;
            text.fontId = getFontId(input.placeholder.FontPath);
            text.size = static_cast<int>(input.placeholder.fontSize);
            text.position = { input.position.x + 10.0f, input.position.y + 10.0f };
            text.z = input.zIndex + 2;
            if (input.value.empty()) {
                text.content = input.placeholder.content;
                applyColor(text, input.placeholderColor);
            } else {
                text.content = input.getDisplayValue();
                applyColor(text, input.textColor);
            }
            _frame.texts.push_back(text);

            if (input.isFocused && input.showCursor) {
                aer::render::RenderRect cursor;
                cursor.position = { text.position.x + 8.0f * static_cast<float>(text.content.size()),
                                    input.position.y + 8.0f };
                cursor.size = { 2.0f, input.size.y - 16.0f };
                cursor.filled = true;
                cursor.z = input.zIndex + 3;
                applyColor(cursor, input.textColor);
                _frame.shapes.push_back(cursor);
            }
        }
    }

    std::uint32_t UIRenderSystem::getFontId(const std::string& path)
    {
        const auto it = _fontCache.find(path);
        if (it != _fontCache.end()) {
            return it->second;
        }
        const auto id = _renderer.loadFont(path);
        _fontCache.emplace(path, id);
        return id;
    }
} // namespace rtp::client::systems
