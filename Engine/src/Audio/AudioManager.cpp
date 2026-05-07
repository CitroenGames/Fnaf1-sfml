#include "Audio/AudioManager.h"

#include "Assets/Resources.h"

AudioManager &AudioManager::GetInstance() {
    static AudioManager instance;
    return instance;
}

void AudioManager::PlayMusic(const std::string &id, bool loop, float volume) {
    if (const auto music = GetMusic(id)) {
        music->setLoop(loop);
        music->setVolume(volume);
        music->play();
        m_ActiveMusic.push_back(music);
    }
}

void AudioManager::PlaySFX(const std::string &id, float volume) {
    if (const auto sound = GetMusic(id)) {
        sound->setVolume(volume);
        sound->setLoop(false);
        sound->play();
        m_ActiveSounds.push_back(sound);
    }
}

void AudioManager::StopAllAudio() {
    for (const auto &music: m_ActiveMusic) {
        if (music) {
            music->stop();
        }
    }
    m_ActiveMusic.clear();

    for (auto &sound: m_ActiveSounds) {
        if (sound) {
            sound->stop();
        }
    }
    m_ActiveSounds.clear();
}

void AudioManager::StopMusic(const std::string &id) {
    if (const auto music = GetMusic(id)) {
        music->stop();
        std::erase(m_ActiveMusic, music);
    }
}

void AudioManager::Preload() {
}

AudioManager::AudioManager() {
}

std::shared_ptr<AudioClip> AudioManager::GetMusic(const std::string &id) {
    return Resources::GetMusic(id);
}
