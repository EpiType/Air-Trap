/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** Slider - UI component for value selection
*/

#pragma once

#include <functional>
#include "RType/Math/Vec2.hpp"

namespace rtp::ecs::components::ui {

/**
 * @struct Slider
 * @brief Component for a draggable slider control
 */
struct Slider {
    Vec2f position;                    /**< Position of the slider */
    Vec2f size{200.0f, 20.0f};        /**< Size of the slider */
    float minValue{0.0f};              /**< Minimum value */
    float maxValue{1.0f};              /**< Maximum value */
    float currentValue{0.5f};          /**< Current value */
    std::function<void(float)> onChange; /**< Callback when value changes */
    
    // Visual properties
    uint8_t trackColor[3]{80, 80, 80};     /**< RGB color of track */
    uint8_t fillColor[3]{100, 200, 100};   /**< RGB color of filled part */
    uint8_t handleColor[3]{200, 200, 200}; /**< RGB color of handle */
    
    bool isDragging{false};            /**< Is currently being dragged */
    int zIndex{5};                     /**< Rendering order */
    
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
