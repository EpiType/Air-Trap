/**
 * File   : MenuScene.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 16/01/2026
 */

#ifndef RTYPE_CLIENT_MENU_SCENE_HPP_
    #define RTYPE_CLIENT_MENU_SCENE_HPP_

    /* Engine */
    #include "engine/scenes/IScene.hpp"
    #include "engine/ecs/Registry.hpp"
    #include "engine/ui/UiFactory.hpp"

    /* R-Type Client */
    #include "rtype/utility/Settings.hpp"
    #include "rtype/utility/TranslationManager.hpp"

    #include <functional>
    #include <string>

namespace rtp::client::scenes
{
    /**
     * @class MenuScene
     * @brief Scene for the main menu.
     */
    class MenuScene : public engine::scenes::IScene
    {
        public:
            MenuScene(engine::ecs::Registry& uiRegistry,
                      Settings& settings,
                      TranslationManager& translation,
                      std::function<void()> onPlay = {},
                      std::function<void()> onSettings = {},
                      std::function<void()> onLogout = {});

            void onEnter(void) override;
            void onExit(void) override;
            void handleEvent(const engine::input::Event &event) override;
            void update(float dt) override;

        private:
            engine::ecs::Registry& _uiRegistry;
            Settings& _settings;
            TranslationManager& _translation;
            std::function<void()> _onPlay;
            std::function<void()> _onSettings;
            std::function<void()> _onLogout;
    };
}

#endif /* !RTYPE_CLIENT_MENU_SCENE_HPP_ */
