/**
 * File   : ModManager.hpp
 * License: MIT
 * Author : GitHub Copilot
 * Date   : 14/01/2026
 */

#ifndef RTYPE_CLIENT_MOD_MODMANAGER_HPP_
#define RTYPE_CLIENT_MOD_MODMANAGER_HPP_

#include <string>
#include <unordered_map>
#include <vector>
#include <optional>
#include <SFML/Graphics/Rect.hpp>

namespace rtp::client {

/**
 * @struct ModEntry
 * @brief Represents a single mod entry mapping an asset to a replacement file
 */
struct ModEntry {
    std::string assetId;        // e.g., "enemy_rt1_3"
    std::string originalPath;   // e.g., "assets/sprites/r-typesheet1.gif"
    std::string moddedPath;     // e.g., "assets/mods/enemy_rt1_3.png"
    int rectLeft;
    int rectTop;
    int rectWidth;
    int rectHeight;
    bool enabled;
};

/**
 * @class ModManager
 * @brief Manages texture/sprite mods for the game
 */
class ModManager {
public:
    ModManager();
    ~ModManager() = default;

    /**
     * @brief Load mods from configuration file
     * @param configPath Path to mods.cfg
     * @return true if loaded successfully
     */
    bool loadConfig(const std::string& configPath = "config/mods.cfg");

    /**
     * @brief Save current mods to configuration file
     * @param configPath Path to mods.cfg
     * @return true if saved successfully
     */
    bool saveConfig(const std::string& configPath = "config/mods.cfg");

    /**
     * @brief Register a mod for a specific sprite
     * @param assetId Asset identifier (e.g., "enemy_rt1_3")
     * @param moddedPath Path to the modded texture
     * @param originalPath Original spritesheet path
     * @param rectLeft Left coordinate of sprite in spritesheet
     * @param rectTop Top coordinate of sprite in spritesheet
     * @param rectWidth Width of sprite
     * @param rectHeight Height of sprite
     */
    void registerMod(const std::string& assetId, const std::string& moddedPath,
                     const std::string& originalPath, int rectLeft, int rectTop,
                     int rectWidth, int rectHeight);

    /**
     * @brief Register a mod with simplified parameters (auto-detect rect from AssetManager)
     * @param assetId Asset identifier (e.g., "enemy_rt1_3")
     * @param moddedPath Path to the modded texture
     * @param rectHint Rectangle hint (use {0,0,0,0} for auto-detect)
     */
    void registerMod(const std::string& assetId, const std::string& moddedPath,
                     const sf::IntRect& rectHint);

    /**
     * @brief Remove a mod
     * @param assetId Asset identifier to remove
     */
    void removeMod(const std::string& assetId);

    /**
     * @brief Enable or disable a mod
     * @param assetId Asset identifier
     * @param enabled Whether the mod should be enabled
     */
    void setModEnabled(const std::string& assetId, bool enabled);

    /**
     * @brief Get the effective texture path for rendering
     * @param originalPath Original texture path
     * @param rectLeft Left coordinate in spritesheet
     * @param rectTop Top coordinate in spritesheet
     * @param rectWidth Width of sprite
     * @param rectHeight Height of sprite
     * @return Modded path if a mod exists and is enabled, otherwise original path
     */
    std::string getEffectivePathByRect(const std::string& originalPath, 
                                       int rectLeft, int rectTop,
                                       int rectWidth, int rectHeight) const;

    /**
     * @brief Get all registered mods
     * @return Vector of all mod entries
     */
    const std::vector<ModEntry>& getAllMods() const;

    /**
     * @brief Get a specific mod by asset ID
     * @param assetId Asset identifier
     * @return ModEntry if found, std::nullopt otherwise
     */
    std::optional<ModEntry> getMod(const std::string& assetId) const;

private:
    std::vector<ModEntry> _mods;
    std::unordered_map<std::string, size_t> _assetIdToIndex;
};

} // namespace rtp::client

#endif // RTYPE_CLIENT_MOD_MODMANAGER_HPP_
