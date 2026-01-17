/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** SoundEvent - Component for one-time sound effects
*/

#pragma once

#include <string>

namespace rtp::ecs::components::audio {

/**
 * @struct SoundEvent
 * @brief Component for triggering one-time sound effects (SFX)
 * @details Used for events like gunshots, explosions, impacts.
 * The AudioSystem plays the sound once and can optionally remove the component.
 */
struct SoundEvent {
    std::string soundPath;           /**< Path to the sound file */
    float volume{1.0f};              /**< Volume level (0.0 - 1.0) */
    float pitch{1.0f};               /**< Pitch multiplier */
    bool played{false};              /**< Whether the sound has been triggered */
};

}  // namespace rtp::ecs::components::audio
