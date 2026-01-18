/**
 * File   : SpriteCustomizer.hpp
 * License: MIT
 * Author : GitHub Copilot
 * Date   : 14/01/2026
 */

#ifndef RTYPE_CLIENT_GAME_SPRITECUSTOMIZER_HPP_
    #define RTYPE_CLIENT_GAME_SPRITECUSTOMIZER_HPP_

    #include <string>
    #include <unordered_map>
    #include <filesystem>
    #include "RType/Config/SpriteMapConfig.hpp"

namespace rtp::client {

    /**
     * @class SpriteCustomizer
     * @brief Singleton class to manage custom sprite mappings globally
     * 
     * This class allows the game to check if an entity has a custom sprite
     * and provides the path to that custom sprite if it exists.
     */
    class SpriteCustomizer {
        public:
            /**
             * @brief Get the singleton instance
             * @return Reference to the singleton instance
             */
            static SpriteCustomizer& getInstance();

            /**
             * @brief Load sprite mappings from config file
             * @return true if successful, false otherwise
             */
            bool loadMappings();

            /**
             * @brief Reload sprite mappings (useful after modifications)
             * This clears the cache and reloads from disk
             */
            void reloadMappings();

            /**
             * @brief Check if an entity has a custom sprite
             * @param entityName Name of the entity
             * @return true if custom sprite exists, false otherwise
             */
            bool hasCustomSprite(const std::string& entityName) const;

            /**
             * @brief Clear all texture caches in render systems
             * This should be called when sprites are modified to ensure
             * that cached textures are reloaded with the new sprites
             */
            static void clearTextureCaches();

            /**
             * @brief Set render system pointers for cache clearing
             * Called internally by Application during initialization
             * @param worldRenderSystem Pointer to world render system
             * @param uiRenderSystem Pointer to UI render system
             */
            static void setRenderSystems(void* worldRenderSystem, void* uiRenderSystem);

            /**
             * @brief Get the custom sprite path for an entity
             * @param entityName Name of the entity
             * @param defaultPath Default sprite path (returned if no custom sprite exists)
             * @return Path to custom sprite, or defaultPath if no custom exists
             */
            std::string getSpritePath(const std::string& entityName, const std::string& defaultPath) const;

            /**
             * @brief Get custom sprite info (path and coordinates for full sprite)
             * @param entityName Name of the entity
             * @param defaultPath Default sprite path
             * @param outLeft Output rect left position (set to 0 if custom sprite)
             * @param outTop Output rect top position (set to 0 if custom sprite)
             * @param outWidth Output width (set to texture width if custom sprite)
             * @param outHeight Output height (set to texture height if custom sprite)
             * @return Path to use (custom or default)
             */
            std::string getSpriteInfo(const std::string& entityName, 
                                     const std::string& defaultPath,
                                     int& outLeft, int& outTop,
                                     int& outWidth, int& outHeight) const;

        private:
            SpriteCustomizer();
            ~SpriteCustomizer() = default;

            // Delete copy/move constructors and assignment operators
            SpriteCustomizer(const SpriteCustomizer&) = delete;
            SpriteCustomizer& operator=(const SpriteCustomizer&) = delete;
            SpriteCustomizer(SpriteCustomizer&&) = delete;
            SpriteCustomizer& operator=(SpriteCustomizer&&) = delete;

            std::filesystem::path _spriteMappingPath;
            std::unordered_map<std::string, std::string> _customSpriteMappings;
    };

} // namespace rtp::client

#endif // RTYPE_CLIENT_GAME_SPRITECUSTOMIZER_HPP_
