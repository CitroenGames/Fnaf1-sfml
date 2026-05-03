#pragma once

#include <memory>
#include <string>
#include <vector>

#include <SFML/Audio/Music.hpp>
#include <SFML/Audio/Sound.hpp>

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

    std::shared_ptr<sf::Music> GetMusic(const std::string &id);
    std::shared_ptr<sf::Sound> GetSound(const std::string &id);

    std::vector<std::shared_ptr<sf::Music> > m_ActiveMusic;
    std::vector<std::shared_ptr<sf::Sound> > m_ActiveSounds;
};
