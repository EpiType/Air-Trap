/**
 * File   : PlayingScene.hpp
 * License: MIT
 * Author : Elias Josue HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 */

#ifndef RTYPE_CLIENT_PLAYING_SCENE_HPP_
    #define RTYPE_CLIENT_PLAYING_SCENE_HPP_

    /* Engine */
    #include "engine/scenes/IScene.hpp"
    #include "engine/ecs/Registry.hpp"
    #include "engine/ecs/Entity.hpp"
    #include "engine/ui/UiFactory.hpp"

    /* R-Type Client */
    #include "rtype/utility/Settings.hpp"
    #include "rtype/utility/TranslationManager.hpp"
    #include "rtype/utility/SceneId.hpp"
    #include "rtype/utility/KeyAction.hpp"
    #include "rtype/systems/NetworkSyncSystem.hpp"
    #include "rtype/entity/EntityBuilder.hpp"

    #include <functional>

namespace rtp::client::scenes
{
    class PlayingScene : public aer::scenes::IScene
    {
        public:
            PlayingScene(aer::ecs::Registry& worldRegistry,
                         aer::ecs::Registry& uiRegistry,
                         Settings& settings,
                         TranslationManager& translation,
                         NetworkSyncSystem& network,
                         EntityBuilder& worldBuilder,
                         std::function<void(SceneId)> changeScene);

            void onEnter(void) override;
            void onExit(void) override;
            void handleEvent(const aer::input::Event &event) override;
            void update(float dt) override;

        private:
            void spawnParallax(void);
            void sendChatMessage(void);
            void openChat(void);
            void closeChat(void);
            void updateChatHistoryText(void);

            aer::ecs::Registry& _worldRegistry;
            aer::ecs::Registry& _uiRegistry;
            Settings& _settings;
            TranslationManager& _translation;
            NetworkSyncSystem& _networkSystem;
            EntityBuilder& _worldBuilder;
            std::function<void(SceneId)> _changeScene;

            bool _hudInit{false};
            bool _chatOpen{false};

            float _chargeTime{0.0f};
            float _fpsTimer{0.0f};
            uint32_t _fpsFrames{0};
            uint32_t _uiFps{0};
            uint32_t _uiPing{0};
            uint32_t _uiScore{0};

            aer::ecs::Entity _hudPing{};
            aer::ecs::Entity _hudFps{};
            aer::ecs::Entity _hudScore{};
            aer::ecs::Entity _hudEntities{};
            aer::ecs::Entity _hudAmmo{};
            aer::ecs::Entity _hudChargeBar{};

            aer::ecs::Entity _chatCompactPanel{};
            aer::ecs::Entity _chatCompactText{};
            aer::ecs::Entity _chatPanel{};
            aer::ecs::Entity _chatHistoryText{};
            aer::ecs::Entity _chatInput{};
    };
}

#endif /* !RTYPE_CLIENT_PLAYING_SCENE_HPP_ */
