#pragma once

#include "SFML/Audio.hpp"
#include "Animatronic.h"
#include "CameraSystem.h"
#include <memory>
#include <unordered_map>
#include <string>

class AudioManager {
public:
    static AudioManager& Instance() {
        static AudioManager instance;
        return instance;
    }

    void Init();
    void PlayFootsteps(const Animatronic::Location& location);
    void PlayJumpscare(const Animatronic::Type& type);
    void PlayAmbient(const CameraSystem::View& view);
    void PlayDoorSound(bool closing);
    void StopAllSounds();

private:
    AudioManager() = default;
    ~AudioManager() = default;

    // Prevent copying of singleton
    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

    void LoadSounds();
    std::shared_ptr<sf::Sound> GetSound(const std::string& name);

    std::unordered_map<std::string, std::shared_ptr<sf::SoundBuffer>> m_SoundBuffers;
    std::unordered_map<std::string, std::shared_ptr<sf::Sound>> m_Sounds;
};
