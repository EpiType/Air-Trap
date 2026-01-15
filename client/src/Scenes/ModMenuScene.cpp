/**
 * File   : ModMenuScene.cpp
 * License: MIT
 * Author : GitHub Copilot
 * Date   : 14/01/2026
 */

#include "Scenes/ModMenuScene.hpp"
#include "Game/SpriteCustomizer.hpp"
#include <fstream>
#include <iostream>

namespace rtp::client {
    namespace scenes {

        //////////////////////////////////////////////////////////////////////////
        // Public API
        //////////////////////////////////////////////////////////////////////////

        ModMenuScene::ModMenuScene(ecs::Registry& UiRegistry,
                                 Settings& settings,
                                 TranslationManager& translationManager,
                                 graphics::UiFactory& uiFactory,
                                 std::function<void(GameState)> changeState)
            : _uiRegistry(UiRegistry),
              _settings(settings),
              _translationManager(translationManager),
              _uiFactory(uiFactory),
              _changeState(changeState),
              _customSpritesPath("assets/sprites/custom"),
              _spriteMappingPath("config/sprite_mappings.json"),
              _selectedCategoryIndex(0),
              _showingCategoryList(true)
        {
            initializeModDirectory();
            loadEntityCategories();
            loadSpriteMappings();
        }

        void ModMenuScene::onEnter(void)
        {
            log::info("Entering ModMenuScene");
            _showingCategoryList = true;
            _loadedTextures.clear();
            loadSpriteMappings(); // Reload mappings in case they changed
            showCategoryList(); // Display the category list
        }

        void ModMenuScene::onExit(void)
        {
            log::info("Exiting ModMenuScene");
            _loadedTextures.clear();
        }

        void ModMenuScene::handleEvent(const sf::Event& e)
        {
            if (const auto* kp = e.getIf<sf::Event::KeyPressed>()) {
                if (kp->code == sf::Keyboard::Key::Escape) {
                    if (_showingCategoryList) {
                        SpriteCustomizer::getInstance().reloadMappings();
                        _changeState(GameState::Menu);
                    } else {
                        // Return to category list
                        _uiRegistry.clear();
                        _showingCategoryList = true;
                        showCategoryList();
                    }
                }
            }
        }

        void ModMenuScene::update(float dt)
        {
            (void)dt;
        }

        //////////////////////////////////////////////////////////////////////////
        // Private methods
        //////////////////////////////////////////////////////////////////////////

        void ModMenuScene::initializeModDirectory()
        {
            try {
                if (!std::filesystem::exists(_customSpritesPath)) {
                    std::filesystem::create_directories(_customSpritesPath);
                    log::info("Created custom sprites directory: {}", _customSpritesPath.string());
                }
                
                // Create subdirectories for each category
                std::vector<std::string> subdirs = {
                    "players", "enemies", "projectiles", "effects"
                };
                
                for (const auto& subdir : subdirs) {
                    auto path = _customSpritesPath / subdir;
                    if (!std::filesystem::exists(path)) {
                        std::filesystem::create_directories(path);
                    }
                }

                // Create a README file with instructions
                auto readmePath = _customSpritesPath / "README.txt";
                if (!std::filesystem::exists(readmePath)) {
                    std::ofstream readme(readmePath);
                    readme << "Custom Sprites Directory\n";
                    readme << "========================\n\n";
                    readme << "Place your custom sprite files here to replace game sprites.\n\n";
                    readme << "Supported formats: PNG, JPG\n\n";
                    readme << "Categories:\n";
                    readme << "  - players/     : Player ship sprites\n";
                    readme << "  - enemies/     : Enemy ship sprites\n";
                    readme << "  - projectiles/ : Bullet and missile sprites\n";
                    readme << "  - effects/     : Explosion and effect sprites\n\n";
                    readme << "Note: Make sure your sprites have transparent backgrounds (PNG recommended)\n";
                    readme.close();
                }
            } catch (const std::exception& e) {
                log::error("Failed to initialize mod directory: {}", e.what());
            }
        }

        void ModMenuScene::loadEntityCategories()
        {
            // Players category (vaisseaux joueurs)
            EntityCategory players;
            players.name = "Players";
            players.entities = {
                {"Player Ship", "assets/sprites/r-typesheet1.gif", 101, 3, 33, 14}  // rt1_1 - the actual player sprite
            };

            // Enemies category (ennemis)
            EntityCategory enemies;
            enemies.name = "Enemies";
            enemies.entities = {
                {"Enemy Fighter 1", "assets/sprites/r-typesheet1.gif", 101, 3, 33, 14},    // rt1_1
                {"Enemy Fighter 2", "assets/sprites/r-typesheet1.gif", 134, 18, 33, 32},   // rt1_2
                {"Enemy Fighter 3", "assets/sprites/r-typesheet1.gif", 2, 51, 33, 32},     // rt1_3
                {"Enemy Ship 1", "assets/sprites/r-typesheet1.gif", 215, 85, 18, 12},      // rt1_4
                {"Enemy Ship 2", "assets/sprites/r-typesheet1.gif", 232, 103, 17, 12},     // rt1_5
                {"Enemy Ship 3", "assets/sprites/r-typesheet1.gif", 200, 121, 33, 10},     // rt1_6
                {"Enemy Ship 4", "assets/sprites/r-typesheet1.gif", 168, 137, 49, 12},     // rt1_7
                {"Enemy Ship 5", "assets/sprites/r-typesheet1.gif", 104, 171, 81, 14},     // rt1_8
                {"Enemy Drone", "assets/sprites/r-typesheet2.gif", 159, 35, 24, 16}        // rt2_2
            };

            // Projectiles category (projectiles)
            EntityCategory projectiles;
            projectiles.name = "Projectiles";
            projectiles.entities = {
                {"Player Laser", "assets/sprites/r-typesheet2.gif", 300, 58, 18, 6},       // rt2_3 (player bullets)
                {"Missile 1", "assets/sprites/r-typesheet1.gif", 211, 276, 16, 12},        // rt1_12
                {"Basic Laser", "assets/sprites/r-typesheet2.gif", 300, 58, 18, 6},        // rt2_3 (enemy bullets)
                {"Heavy Laser", "assets/sprites/r-typesheet2.gif", 300, 71, 30, 18},       // rt2_4
                {"Plasma Shot", "assets/sprites/r-typesheet2.gif", 266, 94, 17, 10},       // rt2_5
                {"Energy Beam", "assets/sprites/r-typesheet2.gif", 101, 118, 17, 14},      // rt2_6
                {"Special Beam", "assets/sprites/r-typesheet2.gif", 157, 316, 18, 14}      // rt2_7
            };

            // Effects category (explosions/effets)
            EntityCategory effects;
            effects.name = "Effects";
            effects.entities = {
                {"Explosion", "assets/sprites/r-typesheet1.gif", 72, 296, 37, 30}          // rt1_13
            };

            _categories = {players, enemies, projectiles, effects};
        }

        void ModMenuScene::showCategoryList()
        {
            // Title
            _uiFactory.createText(
                _uiRegistry,
                {400.0f, 50.0f},
                "Mod Menu - Entity Categories",
                "assets/fonts/title.ttf",
                56,
                10,
                {2, 100, 100}
            );

            // Instructions
            _uiFactory.createText(
                _uiRegistry,
                {100.0f, 150.0f},
                "Select a category to view and customize entity sprites",
                "assets/fonts/main.ttf",
                24,
                5,
                {255, 255, 255}
            );

            // Create category buttons with entity counts
            float yPos = 220.0f;
            const float buttonSpacing = 80.0f;

            for (size_t i = 0; i < _categories.size(); ++i) {
                const auto& category = _categories[i];
                std::string buttonText = category.name + " (" + std::to_string(category.entities.size()) + ")";
                
                _uiFactory.createButton(
                    _uiRegistry,
                    {100.0f, yPos},
                    {600.0f, 60.0f},
                    buttonText,
                    [this, i]() {
                        _selectedCategoryIndex = i;
                        _showingCategoryList = false;
                        _uiRegistry.clear();
                        showEntityCodex(i);
                    }
                );
                yPos += buttonSpacing;
            }

            // Back button
            _uiFactory.createButton(
                _uiRegistry,
                {490.0f, 650.0f},
                {300.0f, 60.0f},
                _translationManager.get("settings.back"),
                [this]() {
                    SpriteCustomizer::getInstance().reloadMappings();
                    _changeState(GameState::Menu);
                }
            );
        }

        void ModMenuScene::showEntityCodex(size_t categoryIndex)
        {
            if (categoryIndex >= _categories.size()) {
                log::error("Invalid category index: {}", categoryIndex);
                return;
            }

            const auto& category = _categories[categoryIndex];

            // Title
            _uiFactory.createText(
                _uiRegistry,
                {400.0f, 30.0f},
                category.name + " Codex",
                "assets/fonts/title.ttf",
                48,
                10,
                {2, 100, 100}
            );

            // Display entities in a grid
            const float startX = 100.0f;
            const float startY = 120.0f;
            const float spacingX = 200.0f;
            const float spacingY = 240.0f;
            const int itemsPerRow = 6;

            for (size_t i = 0; i < category.entities.size(); ++i) {
                const auto& entity = category.entities[i];
                
                int row = i / itemsPerRow;
                int col = i % itemsPerRow;
                
                float x = startX + col * spacingX;
                float y = startY + row * spacingY;

                // Check if entity has custom sprite
                std::string entityKey = getEntityKey(entity);
                std::string displayTexturePath = entity.texturePath;
                int displayLeft = entity.rectLeft;
                int displayTop = entity.rectTop;
                int displayWidth = entity.rectWidth;
                int displayHeight = entity.rectHeight;

                // If custom sprite exists, use it instead
                auto customIt = _customSpriteMappings.find(entityKey);
                if (customIt != _customSpriteMappings.end()) {
                    displayTexturePath = customIt->second;
                    displayLeft = 0;
                    displayTop = 0;
                    // displayWidth and displayHeight stay the same for scaling
                }

                // Display sprite preview
                _uiFactory.createSpritePreview(
                    _uiRegistry,
                    {x + 50.0f, y + 10.0f},
                    displayTexturePath,
                    displayLeft,
                    displayTop,
                    displayWidth,
                    displayHeight,
                    3.0f,  // scale - bigger for better visibility
                    5      // zIndex
                );

                // Entity name text
                std::string displayName = entity.name;
                if (customIt != _customSpriteMappings.end()) {
                    displayName += " *"; // Mark modded sprites with asterisk
                }
                
                _uiFactory.createText(
                    _uiRegistry,
                    {x, y + 140.0f},
                    displayName,
                    "assets/fonts/main.ttf",
                    14,
                    5,
                    {255, 255, 255}
                );

                // Customize button
                _uiFactory.createButton(
                    _uiRegistry,
                    {x + 10.0f, y + 170.0f},
                    {160.0f, 35.0f},
                    customIt != _customSpriteMappings.end() ? "Reset" : "Customize",
                    [this, entity, categoryName = category.name, customIt]() {
                        if (customIt != _customSpriteMappings.end()) {
                            // Reset to default sprite
                            std::string entityKey = getEntityKey(entity);
                            _customSpriteMappings.erase(entityKey);
                            saveSpriteMappings();
                            // Reload SpriteCustomizer to update global state
                            SpriteCustomizer::getInstance().reloadMappings();
                            
                            // Clear texture caches in render systems
                            SpriteCustomizer::clearTextureCaches();
                            
                            _loadedTextures.clear();
                            _uiRegistry.clear();
                            showEntityCodex(_selectedCategoryIndex);
                            log::info("Reset sprite for: {}", entity.name);
                        } else {
                            // Apply custom sprite
                            applyCustomSprite(entity, categoryName);
                        }
                    }
                );
            }

            // Back button
            _uiFactory.createButton(
                _uiRegistry,
                {490.0f, 650.0f},
                {300.0f, 60.0f},
                _translationManager.get("settings.back"),
                [this]() {
                    _uiRegistry.clear();
                    _showingCategoryList = true;
                    showCategoryList();
                }
            );
        }

        sf::Texture* ModMenuScene::loadTexture(const std::string& texturePath)
        {
            auto it = _loadedTextures.find(texturePath);
            if (it != _loadedTextures.end()) {
                return &it->second;
            }

            sf::Texture texture;
            if (texture.loadFromFile(texturePath)) {
                _loadedTextures[texturePath] = std::move(texture);
                return &_loadedTextures[texturePath];
            }

            log::error("Failed to load texture: {}", texturePath);
            return nullptr;
        }

        std::string ModMenuScene::getEntityKey(const EntitySpriteInfo& entity) const
        {
            // Create unique key from entity name (replace spaces with underscores)
            std::string key = entity.name;
            std::replace(key.begin(), key.end(), ' ', '_');
            return key;
        }

        void ModMenuScene::saveSpriteMappings()
        {
            try {
                nlohmann::json j = nlohmann::json::object();  // Force empty object instead of null
                for (const auto& [key, path] : _customSpriteMappings) {
                    j[key] = path;
                }

                std::ofstream file(_spriteMappingPath);
                if (file.is_open()) {
                    file << j.dump(4);
                    log::info("Saved sprite mappings to {}", _spriteMappingPath.string());
                } else {
                    log::error("Failed to open sprite mapping file for writing");
                }
            } catch (const std::exception& e) {
                log::error("Failed to save sprite mappings: {}", e.what());
            }
        }

        void ModMenuScene::loadSpriteMappings()
        {
            try {
                if (!std::filesystem::exists(_spriteMappingPath)) {
                    log::info("No sprite mapping file found, using defaults");
                    return;
                }

                std::ifstream file(_spriteMappingPath);
                if (file.is_open()) {
                    nlohmann::json j;
                    file >> j;

                    _customSpriteMappings.clear();
                    for (auto& [key, value] : j.items()) {
                        _customSpriteMappings[key] = value.get<std::string>();
                    }

                    log::info("Loaded {} custom sprite mappings", _customSpriteMappings.size());
                } else {
                    log::error("Failed to open sprite mapping file for reading");
                }
            } catch (const std::exception& e) {
                log::error("Failed to load sprite mappings: {}", e.what());
            }
        }

        std::string ModMenuScene::openFileDialog()
        {
#ifdef _WIN32
            // Windows: use PowerShell to show file dialog
            std::string command = R"(powershell -Command "Add-Type -AssemblyName System.Windows.Forms; $f = New-Object System.Windows.Forms.OpenFileDialog; $f.Filter = 'Images|*.png;*.jpg;*.jpeg;*.gif'; $f.Title = 'Select Custom Sprite'; if($f.ShowDialog() -eq 'OK'){$f.FileName}")";
            
            FILE* pipe = _popen(command.c_str(), "r");
            if (!pipe) {
                log::error("Failed to open file dialog");
                return "";
            }

            char buffer[512];
            std::string result;
            if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                result = buffer;
                // Remove trailing newline/carriage return
                while (!result.empty() && (result.back() == '\n' || result.back() == '\r')) {
                    result.pop_back();
                }
            }
            _pclose(pipe);

            return result;
#else
            // Linux: use zenity for file dialog
            std::string command = "zenity --file-selection --title='Select Custom Sprite' --file-filter='Images | *.png *.jpg *.jpeg *.gif'";
            
            FILE* pipe = popen(command.c_str(), "r");
            if (!pipe) {
                log::error("Failed to open file dialog");
                return "";
            }

            char buffer[512];
            std::string result;
            if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                result = buffer;
                // Remove trailing newline
                if (!result.empty() && result.back() == '\n') {
                    result.pop_back();
                }
            }
            pclose(pipe);

            return result;
#endif
        }

        sf::Image ModMenuScene::resizeImage(const sf::Image& sourceImage, unsigned int targetWidth, unsigned int targetHeight)
        {
            sf::Vector2u sourceSize = sourceImage.getSize();
            sf::Image resizedImage(sf::Vector2u(targetWidth, targetHeight));

            // Simple nearest-neighbor scaling
            for (unsigned int y = 0; y < targetHeight; ++y) {
                for (unsigned int x = 0; x < targetWidth; ++x) {
                    unsigned int srcX = (x * sourceSize.x) / targetWidth;
                    unsigned int srcY = (y * sourceSize.y) / targetHeight;
                    resizedImage.setPixel(sf::Vector2u(x, y), sourceImage.getPixel(sf::Vector2u(srcX, srcY)));
                }
            }

            return resizedImage;
        }

        void ModMenuScene::applyCustomSprite(const EntitySpriteInfo& entity, const std::string& categoryName)
        {
            // Open file dialog
            std::string selectedFile = openFileDialog();
            if (selectedFile.empty()) {
                log::info("No file selected");
                return;
            }

            log::info("Selected sprite file: {}", selectedFile);

            try {
                // Load the selected image
                sf::Image customImage;
                if (!customImage.loadFromFile(selectedFile)) {
                    log::error("Failed to load custom sprite: {}", selectedFile);
                    return;
                }

                // Resize to match original sprite dimensions
                sf::Image resizedImage = resizeImage(customImage, entity.rectWidth, entity.rectHeight);

                // Create category directory if needed
                std::string categoryLower = categoryName;
                std::transform(categoryLower.begin(), categoryLower.end(), categoryLower.begin(), ::tolower);
                auto categoryPath = _customSpritesPath / categoryLower;
                if (!std::filesystem::exists(categoryPath)) {
                    std::filesystem::create_directories(categoryPath);
                }

                // Save to custom sprites directory with entity name
                std::string entityKey = getEntityKey(entity);
                auto outputPath = categoryPath / (entityKey + ".png");
                
                if (resizedImage.saveToFile(outputPath.string())) {
                    log::info("Saved custom sprite to: {}", outputPath.string());
                    
                    // Update mapping
                    _customSpriteMappings[entityKey] = outputPath.string();
                    saveSpriteMappings();

                    // Reload SpriteCustomizer to update global state
                    SpriteCustomizer::getInstance().reloadMappings();
                    
                    // Clear texture caches in render systems
                    SpriteCustomizer::clearTextureCaches();

                    // Clear texture cache so it reloads
                    _loadedTextures.clear();

                    // Refresh the view
                    _uiRegistry.clear();
                    showEntityCodex(_selectedCategoryIndex);

                    log::info("Custom sprite applied successfully!");
                } else {
                    log::error("Failed to save resized sprite");
                }
            } catch (const std::exception& e) {
                log::error("Error applying custom sprite: {}", e.what());
            }
        }

    } // namespace scenes
} // namespace rtp::client
