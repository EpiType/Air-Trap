/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** AssetManager
*/

#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <unordered_map>

namespace Client::Graphics {
/**
 * @class AssetManager
 * @brief Manages loading and storage of graphical assets.
 * This class handles loading textures and fonts from files
 * and provides access to them via string identifiers.
 */
class AssetManager {
   public:
    AssetManager() = default;
    ~AssetManager() = default;

    void loadTexture(const std::string& id, const std::string& filePath);
    void loadFont(const std::string& id, const std::string& filePath);

    sf::Texture& getTexture(const std::string& id);
    sf::Font& getFont(const std::string& id);

   private:
    std::unordered_map<std::string, sf::Texture> _textures;
    std::unordered_map<std::string, sf::Font> _fonts;
};
}  // namespace Client::Graphics