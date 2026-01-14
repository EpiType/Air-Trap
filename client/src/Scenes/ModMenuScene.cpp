/**
 * File   : ModMenuScene.cpp
 * License: MIT
 * Author : GitHub Copilot  
 * Date   : 14/01/2026
 */

#include "Scenes/ModMenuScene.hpp"
#include "Mod/ModManager.hpp"
#include "RType/Logger.hpp"
#include <filesystem>
#include <cstdlib>

namespace rtp::client {
    namespace Scenes {

        ModMenuScene::ModMenuScene(ModManager& modManager,
                                   ecs::Registry& uiRegistry,
                                   graphics::UiFactory& uiFactory,
                                   std::function<void()> onBack)
            : _modManager(modManager)
            , _uiRegistry(uiRegistry)
            , _uiFactory(uiFactory)
            , _onBack(onBack)
            , _viewMode(ViewMode::Categories) {
            
            // Define sprite categories
            _categories = {
                "Enemies",
                "Players", 
                "Effects",
                "Powerups",
                "Bullets"
            };
        }

        void ModMenuScene::onEnter() {
            rtp::log::info("Entering ModMenuScene");
            createCategoryButtons();
        }

        void ModMenuScene::onExit() {
            rtp::log::info("Exiting ModMenuScene");
        }

        void ModMenuScene::createCategoryButtons() {
            _viewMode = ViewMode::Categories;
            
            // Title
            _uiFactory.createText(
                _uiRegistry,
                {400.0f, 50.0f},
                "SPRITE MODS",
                "assets/fonts/title.ttf",
                60,
                10,
                {255, 100, 100}
            );
            
            // Subtitle
            _uiFactory.createText(
                _uiRegistry,
                {400.0f, 120.0f},
                "Select a category:",
                "assets/fonts/title.ttf",
                30,
                9,
                {200, 200, 200}
            );
            
            // Category buttons
            float yPos = 200.0f;
            for (const auto& category : _categories) {
                _uiFactory.createButton(
                    _uiRegistry,
                    {440.0f, yPos},
                    {400.0f, 60.0f},
                    category,
                    [this, category]() {
                        createSpriteList(category);
                    }
                );
                yPos += 80.0f;
            }
            
            // Back button
            _uiFactory.createButton(
                _uiRegistry,
                {440.0f, yPos + 20.0f},
                {400.0f, 60.0f},
                "BACK TO MENU",
                [this]() {
                    _onBack();
                }
            );
        }

        void ModMenuScene::createSpriteList(const std::string& category) {
            _currentCategory = category;
            _viewMode = ViewMode::SpriteList;
            
            // Clear previous UI
            _uiRegistry.clear();
            
            // Title
            _uiFactory.createText(
                _uiRegistry,
                {400.0f, 50.0f},
                category + " Sprites",
                "assets/fonts/title.ttf",
                50,
                10,
                {255, 100, 100}
            );
            
            // Info text
            _uiFactory.createText(
                _uiRegistry,
                {400.0f, 120.0f},
                "Click on a sprite to replace it",
                "assets/fonts/title.ttf",
                25,
                9,
                {200, 200, 200}
            );
            
            // List common sprites based on category
            std::vector<std::string> sprites;
            
            if (category == "Enemies") {
                sprites = {
                    "enemy_rt1_1", "enemy_rt1_2", "enemy_rt1_3", "enemy_rt1_4",
                    "enemy_rt2_1", "enemy_rt2_2", "enemy_rt2_3",
                    "enemy_rt3_1", "enemy_rt3_2", "enemy_rt3_3",
                    "enemy_tank", "enemy_boss_1"
                };
            } else if (category == "Players") {
                sprites = {
                    "player_ship", "player_ship_damaged",
                    "player_ship_powered"
                };
            } else if (category == "Effects") {
                sprites = {
                    "explosion_small", "explosion_medium", "explosion_large",
                    "shield_effect", "powerup_glow"
                };
            } else if (category == "Powerups") {
                sprites = {
                    "powerup_speed", "powerup_weapon", "powerup_shield",
                    "powerup_life"
                };
            } else if (category == "Bullets") {
                sprites = {
                    "bullet_player", "bullet_enemy", "bullet_laser",
                    "bullet_missile"
                };
            }
            
            // Create button for each sprite
            float yPos = 180.0f;
            int col = 0;
            for (const auto& sprite : sprites) {
                float xPos = 200.0f + (col * 440.0f);
                
                auto modPath = _modManager.getEffectivePathByRect(
                    "assets/sprites/r-typesheet1.gif", 
                    0, 0, 32, 32
                );
                
                std::string label = sprite;
                if (modPath.find("mods/") != std::string::npos) {
                    label += " [MODDED]";
                }
                
                _uiFactory.createButton(
                    _uiRegistry,
                    {xPos, yPos},
                    {400.0f, 50.0f},
                    label,
                    [this, sprite]() {
                        selectSpriteFile(sprite);
                    }
                );
                
                col++;
                if (col >= 2) {
                    col = 0;
                    yPos += 70.0f;
                }
            }
            
            // Back button
            _uiFactory.createButton(
                _uiRegistry,
                {440.0f, 620.0f},
                {400.0f, 60.0f},
                "BACK",
                [this]() {
                    createCategoryButtons();
                }
            );
        }

        void ModMenuScene::selectSpriteFile(const std::string& assetId) {
            rtp::log::info("Selected sprite for modding: {}", assetId);
            
            // Open file browser using zenity or native file dialog
            std::string command = "zenity --file-selection --file-filter='Images | *.png *.jpg *.jpeg *.gif' --title='Select sprite replacement for " + assetId + "' 2>/dev/null";
            
            FILE* pipe = popen(command.c_str(), "r");
            if (!pipe) {
                rtp::log::error("Failed to open file selection dialog");
                return;
            }
            
            char buffer[512];
            std::string result = "";
            if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                result = buffer;
                // Remove trailing newline
                if (!result.empty() && result[result.length()-1] == '\n') {
                    result.erase(result.length()-1);
                }
            }
            pclose(pipe);
            
            if (!result.empty() && std::filesystem::exists(result)) {
                rtp::log::info("Selected file: {}", result);
                
                // Copy file to mods directory
                std::filesystem::path sourcePath(result);
                std::filesystem::path destPath = "assets/mods/" + assetId + sourcePath.extension().string();
                
                try {
                    std::filesystem::create_directories("assets/mods");
                    std::filesystem::copy_file(sourcePath, destPath, 
                                              std::filesystem::copy_options::overwrite_existing);
                    
                    // Register mod
                    sf::IntRect rect({0, 0}, {0, 0});
                    _modManager.registerMod(assetId, destPath.string(), rect);
                    _modManager.saveConfig("config/mods.cfg");
                    
                    rtp::log::info("✓ Mod registered: {} -> {}", assetId, destPath.string());
                    
                    // Refresh the sprite list
                    createSpriteList(_currentCategory);
                    
                } catch (const std::exception& e) {
                    rtp::log::error("Failed to copy mod file: {}", e.what());
                }
            }
        }

        void ModMenuScene::handleEvent(const sf::Event& event) {
            if (const auto* kp = event.getIf<sf::Event::KeyPressed>()) {
                if (kp->code == sf::Keyboard::Key::Escape) {
                    if (_viewMode == ViewMode::Categories) {
                        _onBack();
                    } else {
                        createCategoryButtons();
                    }
                }
            }
        }

        void ModMenuScene::update(float dt) {
            (void)dt;
        }

    } // namespace Scenes
} // namespace rtp::client
