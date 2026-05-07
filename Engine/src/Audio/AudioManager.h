#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Audio/AudioClip.h"

class AudioManager {
public:
    static AudioManager &GetInstance();

    void PlayMusic(const std::string &id, bool loop = false, float volume = 100.0f);
    void PlaySFX(const std::string &id, float volume = 100.0f);
    void StopAllAudio();
    void StopMusic(const std::string &id);
    void Preload();

private:
    AudioManager();

    std::shared_ptr<AudioClip> GetMusic(const std::string &id);

    std::vector<std::shared_ptr<AudioClip> > m_ActiveMusic;
    std::vector<std::shared_ptr<AudioClip> > m_ActiveSounds;
};
