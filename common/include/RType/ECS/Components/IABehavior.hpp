/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** IABehavior
*/

#pragma once

namespace rtp::ecs::components {
/**
 * @enum IABehaviorType
 * @brief Enum representing different types of AI behaviors.
 */
enum IABehavior {
    Passive,    /**< Passive behavior, does not engage */
    Aggressive, /**< Aggressive behavior, attacks on sight */
    Defensive,  /**< Defensive behavior, protects itself */
    Fleeing     /**< Fleeing behavior, runs away from threats */
};

class IABehaviorComponent {
    public:
        IABehavior behavior{IABehavior::Passive}; /**< The AI behavior type */
        double detectionRange{0.0};        /**< Range within which the AI can detect entities */
};
}  // namespace rtp::ecs::components