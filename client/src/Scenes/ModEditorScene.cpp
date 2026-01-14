/**
 * File   : ModEditorScene.cpp
 * License: MIT
 * Author : GitHub Copilot
 * Date   : 14/01/2026
 */

#include "Scenes/ModEditorScene.hpp"
#include "Mod/ModManager.hpp"
#include "RType/Logger.hpp"

namespace rtp::client {
    namespace Scenes {

        ModEditorScene::ModEditorScene(ModManager& modManager, std::function<void()> onBack)
            : _modManager(modManager)
            , _onBack(onBack) {
        }

        void ModEditorScene::onEnter() {
            rtp::log::info("Entering ModEditorScene");
        }

        void ModEditorScene::onExit() {
            rtp::log::info("Exiting ModEditorScene");
        }

        void ModEditorScene::handleEvent(const sf::Event& event) {
            if (const auto* kp = event.getIf<sf::Event::KeyPressed>()) {
                if (kp->code == sf::Keyboard::Key::Escape) {
                    _onBack();
                }
            }
        }

        void ModEditorScene::update(float dt) {
            (void)dt;
        }

    } // namespace Scenes
} // namespace rtp::client
