#include "AudioManager.h"
#include "Assets/Resources.h"

void AudioManager::Init() {
    LoadSounds();
}

void AudioManager::LoadSounds() {
    // Load footstep sounds
    m_SoundBuffers["footstep_left"] = Resources::GetSoundBuffer("Audio/Footsteps/Run_left.wav");
    m_SoundBuffers["footstep_right"] = Resources::GetSoundBuffer("Audio/Footsteps/Run_right.wav");

    // Load jumpscare sound
    m_SoundBuffers["jumpscare_normal"] = Resources::GetSoundBuffer("Audio/Jumpscare/XSCREAM.wav");
    m_SoundBuffers["jumpscare_golden"] = Resources::GetSoundBuffer("Audio/Jumpscare/XSCREAM2.wav");

    // Load door sounds
    m_SoundBuffers["door_close"] = Resources::GetSoundBuffer("Audio/Office/Door.wav");
    m_SoundBuffers["door_open"] = Resources::GetSoundBuffer("Audio/Office/Door2.wav");

    // Load ambient sounds for each camera
    m_SoundBuffers["ambient_kitchen"] = Resources::GetSoundBuffer("Audio/Ambience/Kitchen_Ambience.wav");
    m_SoundBuffers["ambient_hall"] = Resources::GetSoundBuffer("Audio/Ambience/Hall_Ambience.wav");
}

void AudioManager::PlayFootsteps(const Animatronic::Location& location) {
    // Play appropriate footstep sound based on location
    std::string soundName = (location == Animatronic::Location::LEFT_HALL) ? "footstep_left" : "footstep_right";
    auto sound = GetSound(soundName);
    if (sound) {
        sound->play();
    }
}

void AudioManager::PlayJumpscare(const Animatronic::Type& type) {
    std::string soundName = (type == Animatronic::Type::GOLDEN_FREDDY)
        ? "jumpscare_golden"
        : "jumpscare_normal";

    auto sound = GetSound(soundName);
    if (sound) {
        sound->play();
    }
}

void AudioManager::PlayAmbient(const CameraSystem::View& view) {
    // Play ambient sound based on camera view
    std::string soundName;
    switch (view) {
        case CameraSystem::View::KITCHEN:
            soundName = "ambient_kitchen";
            break;
        case CameraSystem::View::EAST_HALL:
        case CameraSystem::View::WEST_HALL:
            soundName = "ambient_hall";
            break;
        default:
            return;
    }

    auto sound = GetSound(soundName);
    if (sound) {
        sound->play();
    }
}

void AudioManager::PlayDoorSound(bool closing) {
    auto sound = GetSound(closing ? "door_close" : "door_open");
    if (sound) {
        sound->play();
    }
}

void AudioManager::StopAllSounds() {
    for (auto& [name, sound] : m_Sounds) {
        if (sound) {
            sound->stop();
        }
    }
}

std::shared_ptr<sf::Sound> AudioManager::GetSound(const std::string& name) {
    if (m_Sounds.find(name) == m_Sounds.end()) {
        if (m_SoundBuffers.find(name) != m_SoundBuffers.end()) {
            m_Sounds[name] = std::make_shared<sf::Sound>();
            m_Sounds[name]->setBuffer(*m_SoundBuffers[name]);
        }
    }
    return m_Sounds[name];
}
