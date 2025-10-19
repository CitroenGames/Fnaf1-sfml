#pragma once

#include "Assets/Resources.h"

// PURPOSE: this helps when switching scenes to avoid manually stopping music and sound effects
class AudioManager {
public:
    static AudioManager &GetInstance() {
        static AudioManager instance;
        return instance;
    }

    // Play music with optional looping
    void PlayMusic(const std::string &id, bool loop = false, float volume = 100.0f) {
        if (const auto music = GetMusic(id)) {
            music->setLooping(loop);
            music->setVolume(volume);
            music->play();
            m_ActiveMusic.push_back(music);
        }
    }

    // Play sound effect
    void PlaySFX(const std::string &id, float volume = 100.0f) {
        if (const auto sound = GetSound(id)) {
            sound->setVolume(volume);
            sound->play();
            m_ActiveSounds.push_back(sound);
        }
    }

    // Stop all audio immediately
    void StopAllAudio() {
        for (const auto &music: m_ActiveMusic) {
            if (music) music->stop();
        }
        m_ActiveMusic.clear();

        for (auto &sound: m_ActiveSounds) {
            if (sound) sound->stop();
        }
        m_ActiveSounds.clear();
    }

    // Stop specific music
    void StopMusic(const std::string &id) {
        if (const auto music = GetMusic(id)) {
            music->stop();
            std::erase(m_ActiveMusic, music);
        }
    }

    // Preload music and sounds to avoid loading during gameplay
    void Preload() {
        // Add preloading code here
    }

private:
    AudioManager() {
    } // Private constructor for singleton

    // Get or load music
    std::shared_ptr<sf::Music> GetMusic(const std::string &id) {
        return Resources::GetMusic(id);
    }

    // Get or load sound
    std::shared_ptr<sf::Sound> GetSound(const std::string &id) {
        // Implement with your Resources system
        return nullptr;
    }

    std::vector<std::shared_ptr<sf::Music> > m_ActiveMusic;
    std::vector<std::shared_ptr<sf::Sound> > m_ActiveSounds;
};
