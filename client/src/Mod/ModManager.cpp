/**
 * File   : ModManager.cpp
 * License: MIT
 * Author : GitHub Copilot
 * Date   : 14/01/2026
 */

#include "Mod/ModManager.hpp"
#include "RType/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>

namespace rtp::client {

ModManager::ModManager() {
    // Constructor
}

bool ModManager::loadConfig(const std::string& configPath) {
    std::ifstream file(configPath);
    if (!file.is_open()) {
        rtp::log::warning("ModManager: Could not open config file: {}", configPath);
        return false;
    }

    _mods.clear();
    _assetIdToIndex.clear();

    std::string line;
    int lineNum = 0;
    while (std::getline(file, line)) {
        lineNum++;
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }

        // Parse format: assetId=moddedPath
        size_t equalPos = line.find('=');
        if (equalPos == std::string::npos) {
            rtp::log::warning("ModManager: Invalid line {} in {}: {}", lineNum, configPath, line);
            continue;
        }

        std::string assetId = line.substr(0, equalPos);
        std::string moddedPath = line.substr(equalPos + 1);

        // Trim whitespace
        assetId.erase(0, assetId.find_first_not_of(" \t\r\n"));
        assetId.erase(assetId.find_last_not_of(" \t\r\n") + 1);
        moddedPath.erase(0, moddedPath.find_first_not_of(" \t\r\n"));
        moddedPath.erase(moddedPath.find_last_not_of(" \t\r\n") + 1);

        if (assetId.empty() || moddedPath.empty()) {
            continue;
        }

        // Create mod entry (will be filled with details later when registered)
        ModEntry entry;
        entry.assetId = assetId;
        entry.moddedPath = moddedPath;
        entry.originalPath = "";  // Will be set when sprite is first encountered
        entry.rectLeft = 0;
        entry.rectTop = 0;
        entry.rectWidth = 0;
        entry.rectHeight = 0;
        entry.enabled = true;

        _assetIdToIndex[assetId] = _mods.size();
        _mods.push_back(entry);

        rtp::log::info("ModManager: Loaded mod '{}' -> '{}'", assetId, moddedPath);
    }

    rtp::log::info("ModManager: Loaded {} mods from {}", _mods.size(), configPath);
    return true;
}

bool ModManager::saveConfig(const std::string& configPath) {
    std::ofstream file(configPath);
    if (!file.is_open()) {
        rtp::log::error("ModManager: Could not open config file for writing: {}", configPath);
        return false;
    }

    file << "# R-Type Mod Configuration\n";
    file << "# Format: assetId=moddedPath\n";
    file << "#\n";

    for (const auto& mod : _mods) {
        if (mod.enabled) {
            file << mod.assetId << "=" << mod.moddedPath << "\n";
        }
    }

    rtp::log::info("ModManager: Saved {} mods to {}", _mods.size(), configPath);
    return true;
}

void ModManager::registerMod(const std::string& assetId, const std::string& moddedPath,
                             const std::string& originalPath, int rectLeft, int rectTop,
                             int rectWidth, int rectHeight) {
    auto it = _assetIdToIndex.find(assetId);
    if (it != _assetIdToIndex.end()) {
        // Update existing mod
        ModEntry& mod = _mods[it->second];
        mod.moddedPath = moddedPath;
        mod.originalPath = originalPath;
        mod.rectLeft = rectLeft;
        mod.rectTop = rectTop;
        mod.rectWidth = rectWidth;
        mod.rectHeight = rectHeight;
    } else {
        // Create new mod
        ModEntry mod;
        mod.assetId = assetId;
        mod.moddedPath = moddedPath;
        mod.originalPath = originalPath;
        mod.rectLeft = rectLeft;
        mod.rectTop = rectTop;
        mod.rectWidth = rectWidth;
        mod.rectHeight = rectHeight;
        mod.enabled = true;

        _assetIdToIndex[assetId] = _mods.size();
        _mods.push_back(mod);
    }
}

void ModManager::registerMod(const std::string& assetId, const std::string& moddedPath,
                             const sf::IntRect& rectHint) {
    // Simplified registration - use empty path and rect for now
    // In a full implementation, you'd look up the original sprite data from AssetManager
    registerMod(assetId, moddedPath, "", rectHint.position.x, rectHint.position.y, 
                rectHint.size.x, rectHint.size.y);
}

void ModManager::removeMod(const std::string& assetId) {
    auto it = _assetIdToIndex.find(assetId);
    if (it != _assetIdToIndex.end()) {
        size_t index = it->second;
        _mods.erase(_mods.begin() + index);
        _assetIdToIndex.erase(it);

        // Rebuild index map
        _assetIdToIndex.clear();
        for (size_t i = 0; i < _mods.size(); ++i) {
            _assetIdToIndex[_mods[i].assetId] = i;
        }
    }
}

void ModManager::setModEnabled(const std::string& assetId, bool enabled) {
    auto it = _assetIdToIndex.find(assetId);
    if (it != _assetIdToIndex.end()) {
        _mods[it->second].enabled = enabled;
    }
}

std::string ModManager::getEffectivePathByRect(const std::string& originalPath,
                                               int rectLeft, int rectTop,
                                               int rectWidth, int rectHeight) const {
    // Normalize path (remove "assets/" prefix if present for comparison)
    std::string normalizedOriginal = originalPath;
    if (normalizedOriginal.find("assets/") == 0) {
        normalizedOriginal = normalizedOriginal.substr(7); // Remove "assets/"
    }

    // Search for matching mod by rectangle coordinates
    for (const auto& mod : _mods) {
        if (!mod.enabled) continue;

        // Normalize stored path
        std::string normalizedStored = mod.originalPath;
        if (normalizedStored.find("assets/") == 0) {
            normalizedStored = normalizedStored.substr(7);
        }

        // Match by path and rectangle
        if (normalizedStored == normalizedOriginal &&
            mod.rectLeft == rectLeft &&
            mod.rectTop == rectTop &&
            mod.rectWidth == rectWidth &&
            mod.rectHeight == rectHeight) {
            rtp::log::info("✓ Found modded sprite: {} -> {}", mod.assetId, mod.moddedPath);
            return mod.moddedPath;
        }
    }

    // Also try matching by asset ID extracted from path
    // e.g., "enemy_rt1_3" from "r-typesheet1.gif" + rect
    for (const auto& mod : _mods) {
        if (!mod.enabled) continue;
        
        // Simple heuristic: if path contains the asset ID substring
        if (originalPath.find(mod.assetId) != std::string::npos) {
            rtp::log::info("✓ Found modded sprite: {} -> {}", mod.assetId, mod.moddedPath);
            return mod.moddedPath;
        }
    }

    // No matching mod found
    rtp::log::info("Found unmodded sprite: {}", normalizedOriginal);
    return originalPath;
}

const std::vector<ModEntry>& ModManager::getAllMods() const {
    return _mods;
}

std::optional<ModEntry> ModManager::getMod(const std::string& assetId) const {
    auto it = _assetIdToIndex.find(assetId);
    if (it != _assetIdToIndex.end()) {
        return _mods[it->second];
    }
    return std::nullopt;
}

} // namespace rtp::client
