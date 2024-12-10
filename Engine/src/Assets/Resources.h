#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <map>
#include <memory>
#include <string>
#include "Pak.h"

class Resources
{
public:
    // TODO: currently you can only have 1 Pak file attached to the Resources class
    static void BindPakFile(const std::string& pakFilename);
    static void Unload();

    static std::shared_ptr<sf::Texture> GetTexture(const std::string& filename);
    static std::shared_ptr<sf::SoundBuffer> GetSoundBuffer(const std::string& filename);
    static std::shared_ptr<sf::Music> GetMusic(const std::string& filename);

    // Load font from a PAK file and return a shared pointer to sf::Font
    static std::shared_ptr<sf::Font> GetFont(const std::string& filename);

    static void SetPakker(Pakker* pakker) { m_PakHandler = pakker; }

private:
    static std::string m_PakFile;
    static Pakker* m_PakHandler;

    static std::map<std::string, std::shared_ptr<sf::Texture>> m_Textures;
    static std::map<std::string, std::shared_ptr<sf::SoundBuffer>> m_SoundBuffers;
    static std::map<std::string, std::shared_ptr<std::vector<uint8_t>>> m_MusicBuffers;

    // Store the loaded fonts
    static std::map<std::string, std::shared_ptr<sf::Font>> m_Fonts;
};