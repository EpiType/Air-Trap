/**
 * File   : AudioSystem.hpp
 * License: MIT
 * Author : Enzo LOBATO COUTINHO <enzo.lobato-coutinho@epitech.eu>
 * Date   : 17/01/2026
 */

#ifndef RTYPE_AUDIO_SYSTEM_HPP_
#define RTYPE_AUDIO_SYSTEM_HPP_

#include "RType/ECS/ISystem.hpp"
#include "RType/ECS/Registry.hpp"
#include "RType/ECS/Components/Audio/AudioSource.hpp"
#include "RType/ECS/Components/Audio/SoundEvent.hpp"

#include <SFML/Audio.hpp>
#include <unordered_map>
#include <memory>

namespace rtp::client {

/**
 * @class AudioSystem
 * @brief System responsible for handling audio playback (music, SFX, ambient)
 * 
 * Manages two types of audio:
 * - AudioSource: Persistent audio (music, loops, ambient sounds)
 * - SoundEvent: One-time sound effects (gunshots, explosions, impacts)
 */
class AudioSystem : public ecs::ISystem {
public:
    explicit AudioSystem(ecs::Registry& registry);
    ~AudioSystem() override;

    /**
     * @brief Update audio system (called every frame)
     * @param dt Delta time in seconds
     */
    void update(float dt) override;

    /**
     * @brief Set master volume for all audio
     * @param volume Volume level (0.0 - 100.0)
     */
    void setMasterVolume(float volume);

    /**
     * @brief Get current master volume
     * @return Current master volume
     */
    float getMasterVolume() const;

    /**
     * @brief Stop all currently playing sounds
     */
    void stopAllSounds();

private:
    ecs::Registry& _registry;
    float _masterVolume;
    std::unordered_map<uint32_t, std::unique_ptr<sf::Sound>> _activeSounds;
    std::unordered_map<std::string, std::unique_ptr<sf::SoundBuffer>> _soundBuffers;
    std::unordered_map<uint32_t, bool> _loopingSounds;  /**< Track which sounds should loop */
    uint32_t _nextSoundId;

    /**
     * @brief Load or get cached sound buffer
     * @param path Path to the audio file
     * @return Pointer to the sound buffer, or nullptr if loading failed
     */
    sf::SoundBuffer* loadSoundBuffer(const std::string& path);

    /**
     * @brief Update all AudioSource components
     * @param dt Delta time in seconds
     */
    void updateAudioSources(float dt);

    /**
     * @brief Update all SoundEvent components
     * @param dt Delta time in seconds
     */
    void updateSoundEvents(float dt);

    /**
     * @brief Play an AudioSource
     * @param audioSource The audio source component to play
     */
    void playAudioSource(ecs::components::audio::AudioSource& audioSource);

    /**
     * @brief Play a sound effect (one-time)
     * @param soundEvent The sound event component to play
     */
    void playSoundEffect(ecs::components::audio::SoundEvent& soundEvent);

    /**
     * @brief Remove finished non-looping sounds from tracking
     */
    void cleanupFinishedSounds();
};

}  // namespace rtp::client

#endif  // !RTYPE_AUDIO_SYSTEM_HPP_