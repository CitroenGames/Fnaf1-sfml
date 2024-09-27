#include "Resources.h"
#include <iostream>

std::string Resources::m_PakFile;
Pakker* Resources::m_PakHandler = nullptr;
std::map<std::string, std::shared_ptr<sf::Texture>> Resources::m_Textures;
std::map<std::string, std::shared_ptr<sf::SoundBuffer>> Resources::m_SoundBuffers;
std::map<std::string, std::shared_ptr<std::vector<uint8_t>>> Resources::m_MusicBuffers;
std::map<std::string, std::shared_ptr<sf::Font>> Resources::m_Fonts;

void Resources::Load(const std::string& pakFilename) {
    if (m_PakHandler == nullptr) {
        std::cerr << "Resources::Load: Pakker instance not set." << std::endl;
        return;
    }
    m_PakFile = pakFilename;
}

void Resources::Unload() {
    m_Textures.clear();
    m_SoundBuffers.clear();
    m_MusicBuffers.clear();
    m_Fonts.clear();
}

std::shared_ptr<sf::Texture> Resources::GetTexture(const std::string& filename) {
    if (m_Textures.find(filename) == m_Textures.end()) {
        auto textureData = m_PakHandler->LoadFile(m_PakFile, filename);
        if (!textureData) {
            std::cerr << "Resources::GetTexture: Failed to load texture: " << filename << std::endl;
            return nullptr;
        }

        std::shared_ptr<sf::Texture> texture = std::make_shared<sf::Texture>();
        if (!texture->loadFromMemory(textureData->data(), textureData->size())) {
            std::cerr << "Resources::GetTexture: Failed to load texture from memory: " << filename << std::endl;
            return nullptr;
        }

        m_Textures[filename] = texture;
    }

    return m_Textures[filename];
}

std::shared_ptr<sf::SoundBuffer> Resources::GetSoundBuffer(const std::string& filename) {
    if (m_SoundBuffers.find(filename) == m_SoundBuffers.end()) {
        auto soundData = m_PakHandler->LoadFile(m_PakFile, filename);
        if (!soundData) {
            std::cerr << "Resources::GetSoundBuffer: Failed to load sound: " << filename << std::endl;
            return nullptr;
        }

        std::shared_ptr<sf::SoundBuffer> buffer = std::make_shared<sf::SoundBuffer>();
        if (!buffer->loadFromMemory(soundData->data(), soundData->size())) {
            std::cerr << "Resources::GetSoundBuffer: Failed to load sound from memory: " << filename << std::endl;
            return nullptr;
        }

        m_SoundBuffers[filename] = buffer;
    }

    return m_SoundBuffers[filename];
}

std::shared_ptr<sf::Music> Resources::GetMusic(const std::string& filename) {
    if (m_MusicBuffers.find(filename) == m_MusicBuffers.end()) {
        auto musicData = m_PakHandler->LoadFile(m_PakFile, filename);
        if (!musicData) {
            std::cerr << "Resources::GetMusic: Failed to load music: " << filename << std::endl;
            return nullptr;
        }

        m_MusicBuffers[filename] = musicData;
    }

    std::shared_ptr<sf::Music> music = std::make_shared<sf::Music>();
    if (!music->openFromMemory(m_MusicBuffers[filename]->data(), m_MusicBuffers[filename]->size())) {
        std::cerr << "Resources::GetMusic: Failed to open music from memory: " << filename << std::endl;
        return nullptr;
    }

    return music;
}

std::shared_ptr<sf::Font> Resources::GetFont(const std::string& filename) {
    if (m_Fonts.find(filename) == m_Fonts.end()) {
        auto fontData = m_PakHandler->LoadFile(m_PakFile, filename);
        if (!fontData) {
            std::cerr << "Resources::GetFont: Failed to load font: " << filename << std::endl;
            return nullptr;
        }

        std::shared_ptr<sf::Font> font = std::make_shared<sf::Font>();
        if (!font->loadFromMemory(fontData->data(), fontData->size())) {
            std::cerr << "Resources::GetFont: Failed to load font from memory: " << filename << std::endl;
            return nullptr;
        }

        m_Fonts[filename] = font;
    }

    return m_Fonts[filename];
}
