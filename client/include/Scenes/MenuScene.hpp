/**
 * File   : MenuScene.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#ifndef RTYPE_CLIENT_SCENES_MENUSCENE_HPP_
    #define RTYPE_CLIENT_SCENES_MENUSCENE_HPP_

    #include "Interfaces/IScene.hpp"
    #include "RType/ECS/Registry.hpp"
    #include "UI/UiFactory.hpp"
    #include "Utils/GameState.hpp"
    #include "Systems/NetworkSyncSystem.hpp"
    #include "Translation/TranslationManager.hpp"
    #include "Core/Settings.hpp"
    #include <SFML/Graphics.hpp>
    #include <memory>
    #include <functional>
    #include <vector>

namespace rtp::client {
    class EntityBuilder;

    namespace scenes {
        /**
         * @class MenuScene
         * @brief Scene for the main menu.
         * 
         * This scene displays the main menu options such as Start Game,
         * Settings, and Exit. It handles user interactions to navigate
         * through the menu.
         */
        class MenuScene : public interfaces::IScene
        {
            public:
                /**
                 * @brief Constructor for MenuScene
                 * @param registry Reference to the ECS registry
                 * @param settings Reference to the application settings
                 * @param network Reference to the client network
                 * @param window Reference to the SFML render window
                 */
                MenuScene(ecs::Registry& UiRegistry,
                          ecs::Registry& worldRegistry,
                          Settings& settings,
                          TranslationManager& translationManager,
                          NetworkSyncSystem& network,
                          graphics::UiFactory& uiFactory,
                          EntityBuilder& worldEntityBuilder,
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
                struct MenuEnemy {
                    ecs::Entity entity;
                    float speed;
                    float baseY;
                    float amplitude;
                    float frequency;
                    float phase;
                    float resetX;
                };

                ecs::Registry& _uiRegistry;                 /**< Reference to the ECS registry */
                ecs::Registry& _worldRegistry;              /**< Reference to the world ECS registry */
                Settings& _settings;                        /**< Reference to the application settings */
                TranslationManager& _translationManager;    /**< Reference to the translation manager */
                NetworkSyncSystem& _network;                /**< Reference to the client network */
                graphics::UiFactory& _uiFactory;            /**< UI Factory for creating UI components */
                EntityBuilder& _worldEntityBuilder;         /**< World entity builder for menu visuals */
                ChangeStateFn _changeState;                 /**< Function to change the game state */
                ecs::Entity _menuMusicEntity;               /**< Entity for menu background music */
                std::vector<ecs::Entity> _menuWorldEntities; /**< Menu background entities */
                std::vector<MenuEnemy> _menuEnemies;        /**< Moving enemies for menu decor */
                float _menuTime{0.0f};                      /**< Menu animation timer */
                ecs::Entity _weaponNameText;                /**< Entity for weapon name display */
                ecs::Entity _weaponStatsText;               /**< Entity for weapon stats display */

                void updateWeaponDisplay();                 /**< Update weapon name and stats texts */
        };
    } // namespace scenes
} // namespace rtp::client

#endif // RTYPE_CLIENT_SCENES_MENUSCENE_HPP_
