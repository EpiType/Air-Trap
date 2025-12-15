/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** Dropdown - UI component for selection from list
*/

#pragma once

#include <functional>
#include <vector>
#include <string>
#include "RType/Math/Vec2.hpp"

namespace rtp::ecs::components::ui {

/**
 * @struct Dropdown
 * @brief Component for dropdown menu selection
 */
struct Dropdown {
    Vec2f position;                           /**< Position of the dropdown */
    Vec2f size{200.0f, 40.0f};               /**< Size of the dropdown button */
    std::vector<std::string> options;         /**< Available options */
    int selectedIndex{0};                     /**< Currently selected option */
    std::function<void(int)> onSelect;        /**< Callback when option selected */
    
    // Visual properties
    uint8_t bgColor[3]{100, 100, 100};       /**< RGB background color */
    uint8_t hoverColor[3]{120, 120, 120};    /**< RGB hover color */
    uint8_t textColor[3]{255, 255, 255};     /**< RGB text color */
    
    bool isOpen{false};                       /**< Is dropdown menu open */
    int hoveredIndex{-1};                     /**< Currently hovered option */
    int zIndex{10};                           /**< Rendering order */
    
    /**
     * @brief Get currently selected option text
     */
    std::string getSelected() const {
        if (selectedIndex >= 0 && selectedIndex < static_cast<int>(options.size())) {
            return options[selectedIndex];
        }
        return "";
    }
};

}  // namespace rtp::ecs::components::ui
