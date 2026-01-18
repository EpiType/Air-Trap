/**
 * File   : SpriteCustomizer.cpp
 * License: MIT
 * Author : GitHub Copilot
 * Date   : 14/01/2026
 */

#include "Game/SpriteCustomizer.hpp"
#include "Systems/RenderSystem.hpp"
#include "Systems/UIRenderSystem.hpp"
#include "RType/Logger.hpp"

namespace rtp::client {

    SpriteCustomizer& SpriteCustomizer::getInstance()
    {
        static SpriteCustomizer instance;
        return instance;
    }

    SpriteCustomizer::SpriteCustomizer()
        : _spriteMappingPath("config/client/sprite_mappings.json")
    {
        loadMappings();
    }

    bool SpriteCustomizer::loadMappings()
    {
        try {
            if (!std::filesystem::exists(_spriteMappingPath)) {
                log::info("No sprite mapping file found at {}, using defaults", _spriteMappingPath.string());
                return false;
            }

            _customSpriteMappings = rtp::config::loadSpriteMappings(_spriteMappingPath.string());
            
            if (_customSpriteMappings.empty()) {
                log::warning("Sprite mappings file exists but is empty or invalid");
                return false;
            }

            log::info("Loaded {} custom sprite mappings", _customSpriteMappings.size());
            return true;
        } catch (const std::exception& e) {
            log::error("Failed to load sprite mappings: {}", e.what());
            return false;
        }
    }

    void SpriteCustomizer::reloadMappings()
    {
        _customSpriteMappings.clear();
        loadMappings();
        log::info("Sprite mappings reloaded");
    }

    bool SpriteCustomizer::hasCustomSprite(const std::string& entityName) const
    {
        bool found = _customSpriteMappings.find(entityName) != _customSpriteMappings.end();
        // Log disabled to avoid spam - uncomment for debugging:
        // if (!found) {
        //     log::debug("SpriteCustomizer: No custom sprite for '{}'. Available keys: {}",
        //         entityName, _customSpriteMappings.size());
        // }
        return found;
    }

    std::string SpriteCustomizer::getSpritePath(const std::string& entityName, const std::string& defaultPath) const
    {
        auto it = _customSpriteMappings.find(entityName);
        if (it != _customSpriteMappings.end()) {
            return it->second;
        }
        return defaultPath;
    }

    std::string SpriteCustomizer::getSpriteInfo(const std::string& entityName,
                                               const std::string& defaultPath,
                                               int& outLeft, int& outTop,
                                               int& outWidth, int& outHeight) const
    {
        auto it = _customSpriteMappings.find(entityName);
        if (it != _customSpriteMappings.end()) {
            // Custom sprite: use full image (already resized to match original dimensions in ModMenu)
            outLeft = 0;
            outTop = 0;
            // Keep outWidth and outHeight unchanged - custom sprite is already resized to match
            log::debug("SpriteCustomizer: Using custom sprite '{}' with original dimensions {}x{}",
                entityName, outWidth, outHeight);
            
            return it->second;
        }
        
        // Default sprite: use provided coordinates
        return defaultPath;
    }

    // Static variables for render systems
    static void* s_worldRenderSystem = nullptr;
    static void* s_uiRenderSystem = nullptr;

    void SpriteCustomizer::setRenderSystems(void* worldRenderSystem, void* uiRenderSystem)
    {
        s_worldRenderSystem = worldRenderSystem;
        s_uiRenderSystem = uiRenderSystem;
    }

    void SpriteCustomizer::clearTextureCaches()
    {
        try {
            if (s_worldRenderSystem) {
                auto* renderSystem = static_cast<client::RenderSystem*>(s_worldRenderSystem);
                renderSystem->clearTextureCache();
                log::info("SpriteCustomizer: Cleared world render system texture cache");
            }
            
            if (s_uiRenderSystem) {
                auto* uiRenderSystem = static_cast<client::systems::UIRenderSystem*>(s_uiRenderSystem);
                uiRenderSystem->clearTextureCache();
                log::info("SpriteCustomizer: Cleared UI render system texture cache");
            }
        } catch (const std::exception& e) {
            log::error("SpriteCustomizer: Error clearing texture caches: {}", e.what());
        }
    }

} // namespace rtp::client
