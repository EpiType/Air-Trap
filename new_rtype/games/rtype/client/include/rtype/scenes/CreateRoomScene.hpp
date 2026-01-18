/**
 * File   : CreateRoomScene.hpp
 * License: MIT
 * Author : Elias Josue HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 */

#ifndef RTYPE_CLIENT_CREATE_ROOM_SCENE_HPP_
    #define RTYPE_CLIENT_CREATE_ROOM_SCENE_HPP_

    /* Engine */
    #include "engine/scenes/IScene.hpp"
    #include "engine/ecs/Registry.hpp"
    #include "engine/ui/UiFactory.hpp"

    /* R-Type Client */
    #include "rtype/utility/Settings.hpp"
    #include "rtype/utility/TranslationManager.hpp"
    #include "rtype/systems/NetworkSyncSystem.hpp"
    #include "rtype/utility/SceneId.hpp"

    #include <functional>
    #include <string>

namespace rtp::client::scenes
{
    class CreateRoomScene : public aer::scenes::IScene
    {
        public:
            CreateRoomScene(aer::ecs::Registry& uiRegistry,
                            Settings& settings,
                            TranslationManager& translation,
                            NetworkSyncSystem& network,
                            std::function<void(SceneId)> changeScene);

            void onEnter(void) override;
            void onExit(void) override;
            void handleEvent(const aer::input::Event &event) override;
            void update(float dt) override;

        private:
            aer::ecs::Registry& _uiRegistry;
            Settings& _settings;
            TranslationManager& _translation;
            NetworkSyncSystem& _networkSystem;
            std::function<void(SceneId)> _changeScene;

            std::string _uiRoomName{"Room"};
            uint32_t _uiMaxPlayers{4};
            float _uiDifficulty{0.5f};
            float _uiSpeed{1.0f};
            uint32_t _uiDuration{10};
            uint32_t _uiSeed{42};
            uint32_t _uiLevelId{1};
    };
}

#endif /* !RTYPE_CLIENT_CREATE_ROOM_SCENE_HPP_ */
