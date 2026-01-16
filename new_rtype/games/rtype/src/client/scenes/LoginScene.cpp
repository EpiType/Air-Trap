/**
 * File   : LoginScene.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 */

#ifndef RTYPE_CLIENT_SCENES_LOGINSCENE_HPP_
    #define RTYPE_CLIENT_SCENES_LOGINSCENE_HPP_

    /* Engine */
    #include "engine/scenes/IScene.hpp"
    #include "engine/ecs/Registry.hpp"

    /* R-Type Client */
    #include "rtype/client/Settings.hpp"
    #include "rtype/client/TranslationManager.hpp"

namespace rtp::client {
    namespace scenes {
        class LoginScene : public engine::scenes::IScene
        {
            public:
                LoginScene(engine::ecs::Registry& UiRegistry,
                            Settings& settings,
                            TranslationManager& translationManager,
                            NetworkSyncSystem& network,
                            graphics::UiFactory& uiFactory,
                            std::function<void(GameState)> changeState);

                using ChangeStateFn = std::function<void(GameState)>;

                void onEnter(void) override;

                void onExit(void) override;

                void handleEvent(const engine::input::Event &) override;

                void update(float dt) override;

            private:
                engine::ecs::Registry& _uiRegistry;
                Settings& _settings;
                TranslationManager& _translationManager;
                NetworkSyncSystem& _network;
                graphics::UiFactory& _uiFactory;
                ChangeStateFn _changeState;
        };
    }
}

#endif // RTYPE_CLIENT_SCENES_LOGINSCENE_HPP_