/**
 * File   : AudioSystem.cpp
 * License: MIT
 * Author : Enzo LOBATO COUTINHO <enzo.lobato-coutinho@epitech.eu>
 * Date   : 17/01/2026
 */

#include "Systems/AudioSystem.hpp"
#include "RType/Logger.hpp"

namespace rtp::client {

AudioSystem::AudioSystem(ecs::Registry& registry)
    : _registry(registry), _masterVolume(100.0f), _musicVolume(100.0f), _sfxVolume(100.0f), _nextSoundId(0) {}

AudioSystem::~AudioSystem() {
    stopAllSounds();
}

void AudioSystem::update(float dt) {
    updateAudioSources(dt);
    updateSoundEvents(dt);
}

void AudioSystem::setMasterVolume(float volume) {
    _masterVolume = std::clamp(volume, 0.0f, 100.0f);
    
    // Update volume of all currently playing sounds
    for (auto& [id, sound] : _activeSounds) {
        float baseVolume = _soundBaseVolumes[id];
        bool isMusic = _soundIsMusic[id];
        float categoryVolume = isMusic ? _musicVolume : _sfxVolume;
        sound->setVolume(baseVolume * _masterVolume * categoryVolume / 100.0f);
    }
}

float AudioSystem::getMasterVolume() const {
    return _masterVolume;
}

void AudioSystem::setMusicVolume(float volume) {
    _musicVolume = std::clamp(volume, 0.0f, 100.0f);
    
    // Update volume of all currently playing music
    for (auto& [id, sound] : _activeSounds) {
        if (_soundIsMusic[id]) {
            float baseVolume = _soundBaseVolumes[id];
            sound->setVolume(baseVolume * _masterVolume * _musicVolume / 100.0f);
        }
    }
}

void AudioSystem::setSfxVolume(float volume) {
    _sfxVolume = std::clamp(volume, 0.0f, 100.0f);
    
    // Update volume of all currently playing SFX
    for (auto& [id, sound] : _activeSounds) {
        if (!_soundIsMusic[id]) {
            float baseVolume = _soundBaseVolumes[id];
            sound->setVolume(baseVolume * _masterVolume * _sfxVolume / 100.0f);
        }
    }
}

void AudioSystem::stopAllSounds() {
    for (auto& [id, sound] : _activeSounds) {
        sound->stop();
    }
    _activeSounds.clear();
    _loopingSounds.clear();
    _soundBaseVolumes.clear();
    _soundIsMusic.clear();
}

sf::SoundBuffer* AudioSystem::loadSoundBuffer(const std::string& path) {
    if (_soundBuffers.find(path) != _soundBuffers.end()) {
        return _soundBuffers[path].get();
    }

    auto buffer = std::make_unique<sf::SoundBuffer>();
    if (!buffer->loadFromFile(path)) {
        log::error("Failed to load sound: {}", path);
        return nullptr;
    }

    sf::SoundBuffer* result = buffer.get();
    _soundBuffers[path] = std::move(buffer);
    return result;
}

void AudioSystem::updateAudioSources(float) {
    try {
        auto view = _registry.zipView<ecs::components::audio::AudioSource>();
        
        for (auto&& [audioSource] : view) {
            if (audioSource.dirty) {
                // Handle state changes
                if (audioSource.isPlaying) {
                    playAudioSource(audioSource);
                }
                audioSource.dirty = false;
            }
            if (audioSource.loop) {
                auto it = _activeSounds.find(audioSource.sourceId);
                if (it != _activeSounds.end() && it->second->getStatus() == sf::Sound::Status::Stopped) {
                    it->second->play();
                }
            }
            cleanupFinishedSounds();
        }
    } catch (const std::exception& e) {
        log::error("Error updating AudioSources: {}", e.what());
    }
}

void AudioSystem::updateSoundEvents(float) {
    try {
        auto view = _registry.zipView<ecs::components::audio::SoundEvent>();
        
        for (auto&& [soundEvent] : view) {
            if (!soundEvent.played) {
                log::info("Playing sound: {}", soundEvent.soundPath);
                playSoundEffect(soundEvent);
                soundEvent.played = true;
            }
        }
    } catch (const std::exception& e) {
        log::error("Error updating SoundEvents: {}", e.what());
    }
}

void AudioSystem::playAudioSource(ecs::components::audio::AudioSource& audioSource) {
    sf::SoundBuffer* buffer = loadSoundBuffer(audioSource.audioPath);
    if (!buffer) return;

    auto sound = std::make_unique<sf::Sound>(*buffer);
    sound->setVolume(audioSource.volume * _masterVolume * _musicVolume / 100.0f);
    sound->setPitch(audioSource.pitch);
    sound->play();

    audioSource.sourceId = _nextSoundId;
    _loopingSounds[_nextSoundId] = audioSource.loop;
    _soundBaseVolumes[_nextSoundId] = audioSource.volume;
    _soundIsMusic[_nextSoundId] = true;  // AudioSource = Music
    _activeSounds[_nextSoundId++] = std::move(sound);
}

void AudioSystem::playSoundEffect(ecs::components::audio::SoundEvent& soundEvent) {
    sf::SoundBuffer* buffer = loadSoundBuffer(soundEvent.soundPath);
    if (!buffer) return;

    auto sound = std::make_unique<sf::Sound>(*buffer);
    sound->setVolume(soundEvent.volume * _masterVolume * _sfxVolume / 100.0f);
    sound->setPitch(soundEvent.pitch);
    sound->play();

    _soundBaseVolumes[_nextSoundId] = soundEvent.volume;
    _soundIsMusic[_nextSoundId] = false;  // SoundEvent = SFX
    _activeSounds[_nextSoundId++] = std::move(sound);
}

void AudioSystem::cleanupFinishedSounds() {
    for (auto it = _activeSounds.begin(); it != _activeSounds.end();) {
        const uint32_t id = it->first;
        const bool isLooping = (_loopingSounds.find(id) != _loopingSounds.end() && _loopingSounds[id]);
        if (it->second->getStatus() == sf::Sound::Status::Stopped && !isLooping) {
            it = _activeSounds.erase(it);
            _loopingSounds.erase(id);
            _soundBaseVolumes.erase(id);
            _soundIsMusic.erase(id);
        } else {
            ++it;
        }
    }
}

}  // namespace rtp::client
