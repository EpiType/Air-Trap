/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** AudioSource - Component for persistent audio playback
*/

#pragma once

#include <string>
#include <cstdint>

namespace rtp::ecs::components::audio {

/**
 * @struct AudioSource
 * @brief Component for entities that emit continuous sound (music, loops, ambient)
 * @details This component manages persistent audio sources that can be played,
 * paused, stopped, and have their volume adjusted in real-time.
 */
struct AudioSource {
    std::string audioPath;           /**< Path to the audio file */
    float volume{1.0f};              /**< Volume level (0.0 - 1.0) */
    bool loop{false};                /**< Whether the audio should loop */
    bool isPlaying{false};           /**< Current playback state */
    float pitch{1.0f};               /**< Pitch multiplier */
    bool dirty{true};                /**< Flag to indicate changes need to be applied */
    uint32_t sourceId{0};            /**< Internal audio source identifier */
};

}  // namespace rtp::ecs::components::audio
