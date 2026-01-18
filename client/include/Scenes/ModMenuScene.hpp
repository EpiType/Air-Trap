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
    #include "Utils/GameState.hpp"
    #include "Translation/TranslationManager.hpp"
    #include "Core/Settings.hpp"
    #include <SFML/Graphics.hpp>
    #include <memory>
    #include <functional>
    #include <filesystem>
    #include <map>

namespace rtp::client {
    namespace scenes {
        
        /**
         * @struct EntitySpriteInfo
         * @brief Information about an entity's sprite for the mod menu
         */
        struct EntitySpriteInfo {
            std::string name;           /**< Display name of the entity */
            std::string texturePath;    /**< Path to the texture */
            int rectLeft;               /**< Left position in sprite sheet */
            int rectTop;                /**< Top position in sprite sheet */
            int rectWidth;              /**< Width of the sprite */
            int rectHeight;             /**< Height of the sprite */
        };

        /**
         * @struct EntityCategory
         * @brief Category of entities with their sprites
         */
        struct EntityCategory {
            std::string name;                       /**< Category name */
            std::vector<EntitySpriteInfo> entities; /**< List of entities in this category */
        };

        /**
         * @class ModMenuScene
         * @brief Scene for customizing game sprites.
         * 
         * This scene allows players to replace game sprites with custom ones.
         * Players can modify sprites for entities, enemies, projectiles, etc.
         */
        class ModMenuScene : public interfaces::IScene
        {
            public:
                /**
                 * @brief Constructor for ModMenuScene
                 * @param registry Reference to the ECS registry
                 * @param settings Reference to the application settings
                 * @param translationManager Reference to the translation manager
                 * @param uiFactory Reference to the UI factory
                 * @param changeState Function to change the game state
                 */
                ModMenuScene(ecs::Registry& UiRegistry,
                           Settings& settings,
                           TranslationManager& translationManager,
                           graphics::UiFactory& uiFactory,
                           std::function<void(GameState)> changeState);

                /**
                 * @brief Type alias for a function that changes the game state.
                 */
                using ChangeStateFn = std::function<void(GameState)>;

                /**
                 * @brief Called when the scene is entered.
                 */
                void onEnter(void) override;

                /**
                 * @brief Called when the scene is exited.
                 */
                void onExit(void) override;

                /**
                 * @brief Handle an incoming event.
                 * @param event The event to handle.
                 */
                void handleEvent(const sf::Event& event) override;

                /**
                 * @brief Update the scene state.
                 * @param dt Time delta since the last update.
                 */
                void update(float dt) override;
            
            private:
                ecs::Registry& _uiRegistry;                 /**< Reference to the ECS registry */
                Settings& _settings;                        /**< Reference to the application settings */
                TranslationManager& _translationManager;    /**< Reference to the translation manager */
                graphics::UiFactory& _uiFactory;            /**< UI Factory for creating UI components */
                ChangeStateFn _changeState;                 /**< Function to change the game state */

                std::filesystem::path _customSpritesPath;   /**< Path to custom sprites directory */
                std::filesystem::path _spriteMappingPath;   /**< Path to sprite mapping config file */
                std::vector<EntityCategory> _categories;    /**< Available entity categories with their sprites */
                size_t _selectedCategoryIndex;              /**< Currently selected category index */
                bool _showingCategoryList;                  /**< True if showing category list, false if showing entity codex */

                // Sprite display
                std::map<std::string, sf::Texture> _loadedTextures; /**< Cache of loaded textures */
                
                // Custom sprite mappings: entity key -> custom sprite path
                std::unordered_map<std::string, std::string> _customSpriteMappings;

                /**
                 * @brief Initialize the custom sprites directory
                 */
                void initializeModDirectory();

                /**
                 * @brief Load all entity categories and their sprites
                 */
                void loadEntityCategories();

                /**
                 * @brief Show the main category selection screen
                 */
                void showCategoryList();

                /**
                 * @brief Show the entity codex for a specific category
                 * @param categoryIndex Index of the category to display
                 */
                void showEntityCodex(size_t categoryIndex);

                /**
                 * @brief Load a texture if not already loaded
                 * @param texturePath Path to the texture file
                 * @return Pointer to the texture, or nullptr if failed
                 */
                sf::Texture* loadTexture(const std::string& texturePath);

                /**
                 * @brief Open file dialog to select a custom sprite
                 * @return Selected file path, or empty string if cancelled
                 */
                std::string openFileDialog();

                /**
                 * @brief Save custom sprite mappings to config file
                 */
                void saveSpriteMappings();

                /**
                 * @brief Load custom sprite mappings from config file
                 */
                void loadSpriteMappings();

                /**
                 * @brief Apply a custom sprite to an entity
                 * @param entity Entity information
                 * @param categoryName Category name for organizing sprites
                 */
                void applyCustomSprite(const EntitySpriteInfo& entity, const std::string& categoryName);

                /**
                 * @brief Resize an image to match target dimensions
                 * @param sourceImage Source image to resize
                 * @param targetWidth Target width
                 * @param targetHeight Target height
                 * @return Resized image
                 */
                sf::Image resizeImage(const sf::Image& sourceImage, unsigned int targetWidth, unsigned int targetHeight);

                /**
                 * @brief Get the unique key for an entity
                 * @param entity Entity information
                 * @return Unique key string
                 */
                std::string getEntityKey(const EntitySpriteInfo& entity) const;
        };
    } // namespace scenes
} // namespace rtp::client

#endif // RTYPE_CLIENT_SCENES_MODMENUSCENE_HPP_
