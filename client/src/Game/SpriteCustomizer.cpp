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
#include <fstream>

namespace rtp::client {

    SpriteCustomizer& SpriteCustomizer::getInstance()
    {
        static SpriteCustomizer instance;
        return instance;
    }

    SpriteCustomizer::SpriteCustomizer()
        : _spriteMappingPath("config/sprite_mappings.json")
    {
        loadMappings();
    }

    bool SpriteCustomizer::loadMappings()
    {
        try {
            if (!std::filesystem::exists(_spriteMappingPath)) {
                rtp::log::info("No sprite mapping file found at {}, using defaults", _spriteMappingPath.string());
                return false;
            }

            std::ifstream file(_spriteMappingPath);
            if (!file.is_open()) {
                rtp::log::error("Failed to open sprite mapping file: {}", _spriteMappingPath.string());
                return false;
            }

            nlohmann::json j;
            file >> j;

            _customSpriteMappings.clear();
            for (auto& [key, value] : j.items()) {
                _customSpriteMappings[key] = value.get<std::string>();
            }

            rtp::log::info("Loaded {} custom sprite mappings", _customSpriteMappings.size());
            return true;
        } catch (const std::exception& e) {
            rtp::log::error("Failed to load sprite mappings: {}", e.what());
            return false;
        }
    }

    void SpriteCustomizer::reloadMappings()
    {
        _customSpriteMappings.clear();
        loadMappings();
        rtp::log::info("Sprite mappings reloaded");
    }

    bool SpriteCustomizer::hasCustomSprite(const std::string& entityName) const
    {
        bool found = _customSpriteMappings.find(entityName) != _customSpriteMappings.end();
        if (!found) {
            rtp::log::debug("SpriteCustomizer: No custom sprite for '{}'. Available keys: {}",
                entityName, _customSpriteMappings.size());
        }
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
            rtp::log::debug("SpriteCustomizer: Using custom sprite '{}' with original dimensions {}x{}",
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
                auto* renderSystem = static_cast<rtp::client::RenderSystem*>(s_worldRenderSystem);
                renderSystem->clearTextureCache();
                rtp::log::info("SpriteCustomizer: Cleared world render system texture cache");
            }
            
            if (s_uiRenderSystem) {
                auto* uiRenderSystem = static_cast<Client::Systems::UIRenderSystem*>(s_uiRenderSystem);
                uiRenderSystem->clearTextureCache();
                rtp::log::info("SpriteCustomizer: Cleared UI render system texture cache");
            }
        } catch (const std::exception& e) {
            rtp::log::error("SpriteCustomizer: Error clearing texture caches: {}", e.what());
        }
    }

} // namespace rtp::client
