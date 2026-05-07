#pragma once

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>

#include "Audio/AudioClip.h"
#include "Pak.h"

class Resources {
public:
    // Resources currently binds one Pak file at a time.
    static void BindPakFile(const std::string &pakFilename);

    static void Unload();

    static std::shared_ptr<sf::Texture> GetTexture(const std::string &filename);

    static std::shared_ptr<AudioClip> GetMusic(const std::string &filename);

    static std::shared_ptr<sf::Font> GetFont(const std::string &filename);

    static std::vector<std::string> ListFiles();

    static std::vector<std::string> ListFilesWithPrefix(const std::string &prefix);

    static void SetPakker(Pakker *pakker) { m_PakHandler = pakker; }

private:
    static std::string m_PakFile;
    static Pakker *m_PakHandler;

    static std::map<std::string, std::shared_ptr<sf::Texture> > m_Textures;
    static std::map<std::string, std::shared_ptr<AudioBuffer> > m_AudioBuffers;

    static std::map<std::string, std::shared_ptr<sf::Font> > m_Fonts;
    static std::map<std::string, std::shared_ptr<std::vector<uint8_t> > > m_FontBuffers;

    static std::shared_ptr<std::vector<uint8_t> > LoadPakAsset(const std::string &filename,
                                                               const char *errorPrefix);
};
