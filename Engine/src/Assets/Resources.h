#pragma once

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include <SFML/Audio/Music.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>

#include "Pak.h"

class Resources {
public:
    // Resources currently binds one Pak file at a time.
    static void BindPakFile(const std::string &pakFilename);

    static void Unload();

    static std::shared_ptr<sf::Texture> GetTexture(const std::string &filename);

    static std::shared_ptr<sf::SoundBuffer> GetSoundBuffer(const std::string &filename);

    static std::shared_ptr<sf::Music> GetMusic(const std::string &filename);

    static std::shared_ptr<sf::Font> GetFont(const std::string &filename);

    static void SetPakker(Pakker *pakker) { m_PakHandler = pakker; }

private:
    static std::string m_PakFile;
    static Pakker *m_PakHandler;

    static std::map<std::string, std::shared_ptr<sf::Texture> > m_Textures;
    static std::map<std::string, std::shared_ptr<sf::SoundBuffer> > m_SoundBuffers;
    static std::map<std::string, std::shared_ptr<std::vector<uint8_t> > > m_MusicBuffers;

    static std::map<std::string, std::shared_ptr<sf::Font> > m_Fonts;
    static std::map<std::string, std::shared_ptr<std::vector<uint8_t> > > m_FontBuffers;
};
