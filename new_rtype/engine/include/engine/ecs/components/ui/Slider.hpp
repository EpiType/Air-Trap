/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** Slider - UI component for value selection
*/

#ifndef ENGINE_ECS_COMPONENTS_SLIDER_HPP_
    #define ENGINE_ECS_COMPONENTS_SLIDER_HPP_

    #include <functional>
    #include "engine/math/Vec2.hpp"
    #include "engine/ui/UiUtilities.hpp"

namespace engine::ecs::components
{
    /**
     * @struct Slider
     * @brief Component for a draggable slider control
     */
    struct Slider {
        math::Vec2f position;                   /**< Position of the slider */
        math::Vec2f size{200.0f, 20.0f};        /**< Size of the slider */
        float minValue{0.0f};                   /**< Minimum value */
        float maxValue{1.0f};                   /**< Maximum value */
        float currentValue{0.5f};               /**< Current value */
        std::function<void(float)> onChange;    /**< Callback when value changes */
        
        ui::color trackColor{80, 80, 80};       /**< RGB color of track */
        ui::color fillColor{100, 200, 100};     /**< RGB color of filled part */
        ui::color handleColor{200, 200, 200};   /**< RGB color of handle */
        
        bool isDragging{false};                 /**< Is currently being dragged */
        int zIndex{5};                          /**< Rendering order */
        
        /**
         * @brief Get normalized value (0-1)
         */
        float getNormalized() const {
            return (currentValue - minValue) / (maxValue - minValue);
        }
        
        /**
         * @brief Set value from normalized (0-1)
         */
        void setNormalized(float normalized) {
            currentValue = minValue + normalized * (maxValue - minValue);
        }
    };
}  // namespace rtp::ecs::components::ui

#endif /* !ENGINE_ECS_COMPONENTS_SLIDER_HPP_ */
