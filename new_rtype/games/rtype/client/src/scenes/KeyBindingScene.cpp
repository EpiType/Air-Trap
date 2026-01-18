/**
 * File   : KeyBindingScene.cpp
 * License: MIT
 * Author : Elias Josue HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 */

#include "rtype/scenes/KeyBindingScene.hpp"

#include "engine/ecs/components/ui/Button.hpp"

#include <vector>

namespace rtp::client::scenes
{
    namespace
    {
        aer::ui::textContainer makeText(const std::string &content,
                                        const std::string &font,
                                        unsigned int size)
        {
            aer::ui::textContainer text;
            text.content = content;
            text.FontPath = font;
            text.fontSize = size;
            return text;
        }
    }

    KeyBindingScene::KeyBindingScene(aer::ecs::Registry& uiRegistry,
                                     Settings& settings,
                                     TranslationManager& translation,
                                     NetworkSyncSystem& network,
                                     std::function<void(SceneId)> changeScene)
        : _uiRegistry(uiRegistry),
          _settings(settings),
          _translation(translation),
          _networkSystem(network),
          _changeScene(changeScene)
    {
    }

    void KeyBindingScene::onEnter(void)
    {
        aer::ui::UiFactory::createText(
            _uiRegistry,
            {450.0f, 50.0f},
            makeText(_translation.get("keybindings.title"), "assets/fonts/main.ttf", 60),
            10,
            {255, 200, 100}
        );

        float yPos = 150.0f;
        const float buttonSpacing = 60.0f;

        struct KeyBindingButton {
            const char *labelKey;
            KeyAction action;
        };

        const std::vector<KeyBindingButton> bindings = {
            {"keybindings.move_up", KeyAction::MoveUp},
            {"keybindings.move_down", KeyAction::MoveDown},
            {"keybindings.move_left", KeyAction::MoveLeft},
            {"keybindings.move_right", KeyAction::MoveRight},
            {"keybindings.shoot", KeyAction::Shoot},
            {"keybindings.reload", KeyAction::Reload}
        };

        for (const auto& binding : bindings) {
            aer::ui::UiFactory::createText(
                _uiRegistry,
                {200.0f, yPos + 10.0f},
                makeText(_translation.get(binding.labelKey), "assets/fonts/main.ttf", 24),
                22,
                {255, 255, 255}
            );

            const std::string keyName = _settings.getKeyName(_settings.getKey(binding.action));
            const auto entity = aer::ui::UiFactory::createButton(
                _uiRegistry,
                {500.0f, yPos},
                {200.0f, 40.0f},
                makeText(keyName, "assets/fonts/main.ttf", 20),
                [this, actionToBind = binding.action]() {
                    _isWaitingForKey = true;
                    _actionToRebind = actionToBind;
                    refreshButtonLabel(actionToBind);
                }
            );

            _actionToButton[binding.action] = entity;
            yPos += buttonSpacing;
        }

        aer::ui::UiFactory::createButton(
            _uiRegistry,
            {490.0f, 650.0f},
            {300.0f, 60.0f},
            makeText(_translation.get("settings.back"), "assets/fonts/main.ttf", 22),
            [this]() {
                _settings.save();
                if (_changeScene) {
                    _changeScene(SceneId::Settings);
                }
            }
        );
    }

    void KeyBindingScene::onExit(void)
    {
        _uiRegistry.clear();
        _uiRegistry.purge();
        _isWaitingForKey = false;
    }

    void KeyBindingScene::handleEvent(const aer::input::Event &event)
    {
        if (!_isWaitingForKey) {
            return;
        }

        if (event.type != aer::input::EventType::KeyDown) {
            return;
        }

        if (event.key == aer::input::KeyCode::Escape) {
            _isWaitingForKey = false;
            refreshButtonLabel(_actionToRebind);
            return;
        }

        _settings.setKey(_actionToRebind, event.key);
        _settings.save();
        _isWaitingForKey = false;
        refreshButtonLabel(_actionToRebind);
    }

    void KeyBindingScene::update(float dt)
    {
        (void)dt;
        (void)_networkSystem;
    }

    void KeyBindingScene::refreshButtonLabel(KeyAction action)
    {
        auto it = _actionToButton.find(action);
        if (it == _actionToButton.end()) {
            return;
        }

        const auto entity = it->second;
        auto buttonsOpt = _uiRegistry.getComponents<aer::ecs::components::Button>();
        if (!buttonsOpt) {
            return;
        }

        auto &buttons = buttonsOpt.value().get();
        if (!buttons.has(entity)) {
            return;
        }

        const std::string label = _isWaitingForKey ? "Press key..." :
            _settings.getKeyName(_settings.getKey(action));
        buttons[entity].text.content = label;
    }
} // namespace rtp::client::scenes
