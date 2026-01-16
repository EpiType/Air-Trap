/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** Dropdown - UI component for selection from list
*/

#ifndef ENGINE_ECS_COMPONENTS_DROPDOWN_HPP_
    #define ENGINE_ECS_COMPONENTS_DROPDOWN_HPP_

#include <functional>
#include <vector>
#include <string>
#include "engine/math/Vec2.hpp"
#include "engine/ui/UiUtilities.hpp"

namespace engine::ecs::components
{
    /**
     * @struct Dropdown
     * @brief Component for dropdown menu selection
     */
    struct Dropdown {
        math::Vec2f position;                   /**< Position of the dropdown */
        math::Vec2f size{200.0f, 40.0f};        /**< Size of the dropdown button */
        std::vector<std::string> options;       /**< Available options */
        int selectedIndex{0};                   /**< Currently selected option */
        std::function<void(int)> onSelect;      /**< Callback when option selected */
        
        ui::color bgColor{100, 100, 100};       /**< RGB background color */
        ui::color hoverColor{120, 120, 120};    /**< RGB hover color */
        ui::color textColor{255, 255, 255};     /**< RGB text color */
        
        bool isOpen{false};                     /**< Is dropdown menu open */
        int hoveredIndex{-1};                   /**< Currently hovered option */
        int zIndex{10};                         /**< Rendering order */
        
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
}  // namespace engine::ecs::components

#endif /* !ENGINE_ECS_COMPONENTS_DROPDOWN_HPP_ */
