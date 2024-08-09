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
    static void Load(const std::string& pakFilename);
    static void Unload();

    static std::shared_ptr<sf::Texture> GetTexture(const std::string& filename);
    static std::shared_ptr<sf::SoundBuffer> GetSoundBuffer(const std::string& filename);
    
    // Load music from a PAK file and return a shared pointer to sf::Music
    static std::shared_ptr<sf::Music> GetMusic(const std::string& filename);

private:
    static std::string pakFile;
    static Pakker pakHandler;

    static std::map<std::string, std::shared_ptr<sf::Texture>> textures;
    static std::map<std::string, std::shared_ptr<sf::SoundBuffer>> soundBuffers;
    
    // Store the raw data buffers for music files to ensure they persist
    static std::map<std::string, std::shared_ptr<std::vector<uint8_t>>> musicBuffers;
};
