/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** Rect
*/

#ifndef ENGINE_ECS_COMPONENTS_BOUNDINGBOX_HPP_
    #define ENGINE_ECS_COMPONENTS_BOUNDINGBOX_HPP_
 
/**
 * @namespace aer::ecs::components
 * @brief ECS components for R-Type engine
 */
namespace aer::ecs::components
{
    /**
     * @struct Rect
     * @brief Component representing a rectangle area.
     * Used for defining bounding boxes or collision areas.
     */
    struct BoundingBox {
        float width{0.0f};      /**< Width of the rectangle */
        float height{0.0f};     /**< Height of the rectangle */
    };
}  // namespace aer::ecs::components

#endif /* !ENGINE_ECS_COMPONENTS_BOUNDINGBOX_HPP_ */