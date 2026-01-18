/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** Text
*/

#ifndef ENGINE_ECS_COMPONENTS_TEXT_HPP_
    #define ENGINE_ECS_COMPONENTS_TEXT_HPP_

    #include <string>
    #include "engine/math/Vec2.hpp"
    #include "engine/ui/UiUtilities.hpp"

namespace aer::ecs::components
{
    /**
     * @struct Text
     * @brief Component representing text to render
     */
    struct Text {
        ui::textContainer text;              /**< Text content and font info */
        math::Vec2f position;                   /**< Position of the text */
        ui::color color{255, 255, 255, 255};    /**< RGB color of the text */
        int zIndex{0};                          /**< Rendering order */
    };
}  // namespace aer::ecs::components

#endif /* !ENGINE_ECS_COMPONENTS_TEXT_HPP_ */