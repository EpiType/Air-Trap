/**
 * File   : SpriteMapConfig.hpp
 * License: MIT
 * Author : GitHub Copilot
 * Date   : 18/01/2026
 * 
 * @brief Configuration loader for sprite mappings (mod system)
 */

#ifndef RTYPE_CONFIG_SPRITEMAPCONFIG_HPP_
#define RTYPE_CONFIG_SPRITEMAPCONFIG_HPP_

#include "SimpleJsonParser.hpp"
#include <unordered_map>
#include <string>
#include <fstream>
#include <sstream>

namespace rtp::config {

    /**
     * @brief Load sprite mappings from a JSON file
     * @param filepath Path to sprite_mappings.json (default: config/client/sprite_mappings.json)
     * @return Map of entity keys to sprite paths
     */
    inline std::unordered_map<std::string, std::string> loadSpriteMappings(
        const std::string& filepath = "config/client/sprite_mappings.json")
    {
        std::unordered_map<std::string, std::string> mappings;
        
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return mappings; // Return empty map if file doesn't exist
        }

        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
        
        SimpleJson parser;
        if (!parser.parseContent(content)) {
            return mappings; // Return empty map if parse fails
        }

        mappings = parser.getData();
        return mappings;
    }

    /**
     * @brief Save sprite mappings to a JSON file
     * @param mappings Map of entity keys to sprite paths
     * @param filepath Path to sprite_mappings.json (default: config/client/sprite_mappings.json)
     * @return true if saved successfully, false otherwise
     */
    inline bool saveSpriteMappings(
        const std::unordered_map<std::string, std::string>& mappings,
        const std::string& filepath = "config/client/sprite_mappings.json")
    {
        try {
            std::stringstream ss;
            ss << "{\n";
            bool first = true;
            for (const auto& [key, path] : mappings) {
                if (!first) ss << ",\n";
                ss << "  \"" << key << "\": \"" << path << "\"";
                first = false;
            }
            ss << "\n}";

            std::ofstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            file << ss.str();
            return true;
        } catch (...) {
            return false;
        }
    }

} // namespace rtp::config

#endif // RTYPE_CONFIG_SPRITEMAPCONFIG_HPP_
