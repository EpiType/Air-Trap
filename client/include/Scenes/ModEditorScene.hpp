/**
 * File   : ModEditorScene.hpp
 * License: MIT
 * Author : GitHub Copilot
 * Date   : 14/01/2026
 */

#ifndef RTYPE_CLIENT_SCENES_MODEDITORSCENE_HPP_
#define RTYPE_CLIENT_SCENES_MODEDITORSCENE_HPP_

#include "Interfaces/IScene.hpp"
#include <functional>

namespace rtp::client {

    class ModManager;
    
    namespace Scenes {

        class ModEditorScene : public interfaces::IScene {
        public:
            ModEditorScene(ModManager& modManager, std::function<void()> onBack);
            ~ModEditorScene() override = default;

            void onEnter() override;
            void onExit() override;
            void handleEvent(const sf::Event& event) override;
            void update(float dt) override;

        private:
            ModManager& _modManager;
            std::function<void()> _onBack;
        };

    } // namespace Scenes
} // namespace rtp::client

#endif // RTYPE_CLIENT_SCENES_MODEDITORSCENE_HPP_
