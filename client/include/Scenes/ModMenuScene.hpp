/**
 * File   : ModMenuScene.hpp
 * License: MIT
 * Author : GitHub Copilot
 * Date   : 14/01/2026
 */

#ifndef RTYPE_CLIENT_SCENES_MODMENUSCENE_HPP_
#define RTYPE_CLIENT_SCENES_MODMENUSCENE_HPP_

#include "Interfaces/IScene.hpp"
#include "RType/ECS/Registry.hpp"
#include "UI/UiFactory.hpp"
#include <SFML/Graphics.hpp>
#include <functional>
#include <vector>
#include <string>

namespace rtp::client {

    class ModManager;
    
    namespace Scenes {

        class ModMenuScene : public interfaces::IScene {
        public:
            ModMenuScene(ModManager& modManager, 
                        ecs::Registry& uiRegistry,
                        graphics::UiFactory& uiFactory,
                        std::function<void()> onBack);
            ~ModMenuScene() override = default;

            void onEnter() override;
            void onExit() override;
            void handleEvent(const sf::Event& event) override;
            void update(float dt) override;

        private:
            void createCategoryButtons();
            void createSpriteList(const std::string& category);
            void selectSpriteFile(const std::string& assetId);
            
            ModManager& _modManager;
            ecs::Registry& _uiRegistry;
            graphics::UiFactory& _uiFactory;
            std::function<void()> _onBack;
            
            std::string _currentCategory;
            std::vector<std::string> _categories;
            
            enum class ViewMode {
                Categories,
                SpriteList,
                FileSelect
            };
            ViewMode _viewMode;
        };

    } // namespace Scenes
} // namespace rtp::client

#endif // RTYPE_CLIENT_SCENES_MODMENUSCENE_HPP_
