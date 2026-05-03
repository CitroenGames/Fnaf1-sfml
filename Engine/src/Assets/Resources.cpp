#include "Resources.h"

#include <iostream>

std::string Resources::m_PakFile;
Pakker *Resources::m_PakHandler = nullptr;
std::map<std::string, std::shared_ptr<sf::Texture> > Resources::m_Textures;
std::map<std::string, std::shared_ptr<sf::SoundBuffer> > Resources::m_SoundBuffers;
std::map<std::string, std::shared_ptr<std::vector<uint8_t> > > Resources::m_MusicBuffers;
std::map<std::string, std::shared_ptr<sf::Font> > Resources::m_Fonts;
std::map<std::string, std::shared_ptr<std::vector<uint8_t> > > Resources::m_FontBuffers;

std::shared_ptr<std::vector<uint8_t> > Resources::LoadPakAsset(const std::string &filename,
                                                               const char *errorPrefix) {
    if (m_PakHandler == nullptr) {
        std::cerr << "Resources::Load: Pakker instance not set." << std::endl;
        return nullptr;
    }

    auto data = m_PakHandler->LoadFile(m_PakFile, filename);
    if (!data) {
        std::cerr << errorPrefix << filename << std::endl;
    }

    return data;
}

void Resources::BindPakFile(const std::string &pakFilename) {
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
    m_FontBuffers.clear();
}

std::shared_ptr<sf::Texture> Resources::GetTexture(const std::string &filename) {
    auto textureIt = m_Textures.find(filename);
    if (textureIt == m_Textures.end()) {
        const auto textureData = LoadPakAsset(filename, "Resources::GetTexture: Failed to load texture: ");
        if (!textureData) {
            return nullptr;
        }

        auto texture = std::make_shared<sf::Texture>();
        if (!texture->loadFromMemory(textureData->data(), textureData->size())) {
            std::cerr << "Resources::GetTexture: Failed to load texture from memory: " << filename << std::endl;
            return nullptr;
        }

        textureIt = m_Textures.emplace(filename, texture).first;
    }

    return textureIt->second;
}

std::shared_ptr<sf::SoundBuffer> Resources::GetSoundBuffer(const std::string &filename) {
    auto bufferIt = m_SoundBuffers.find(filename);
    if (bufferIt == m_SoundBuffers.end()) {
        const auto soundData = LoadPakAsset(filename, "Resources::GetSoundBuffer: Failed to load sound: ");
        if (!soundData) {
            return nullptr;
        }

        const auto buffer = std::make_shared<sf::SoundBuffer>();
        if (!buffer->loadFromMemory(soundData->data(), soundData->size())) {
            std::cerr << "Resources::GetSoundBuffer: Failed to load sound from memory: " << filename << std::endl;
            return nullptr;
        }

        bufferIt = m_SoundBuffers.emplace(filename, buffer).first;
    }

    return bufferIt->second;
}

std::shared_ptr<sf::Music> Resources::GetMusic(const std::string &filename) {
    auto bufferIt = m_MusicBuffers.find(filename);
    if (bufferIt == m_MusicBuffers.end()) {
        const auto musicData = LoadPakAsset(filename, "Resources::GetMusic: Failed to load music: ");
        if (!musicData) {
            return nullptr;
        }

        bufferIt = m_MusicBuffers.emplace(filename, musicData).first;
    }

    auto music = std::make_shared<sf::Music>();
    if (!music->openFromMemory(bufferIt->second->data(), bufferIt->second->size())) {
        std::cerr << "Resources::GetMusic: Failed to open music from memory: " << filename << std::endl;
        return nullptr;
    }

    return music;
}

std::shared_ptr<sf::Font> Resources::GetFont(const std::string &filename) {
    auto fontIt = m_Fonts.find(filename);
    if (fontIt == m_Fonts.end()) {
        const auto fontData = LoadPakAsset(filename, "Resources::GetFont: Failed to load font: ");
        if (!fontData) {
            return nullptr;
        }

        auto font = std::make_shared<sf::Font>();
        if (!font->loadFromMemory(fontData->data(), fontData->size())) {
            std::cerr << "Resources::GetFont: Failed to load font from memory: " << filename << std::endl;
            return nullptr;
        }

        m_FontBuffers[filename] = fontData;
        fontIt = m_Fonts.emplace(filename, font).first;
    }

    return fontIt->second;
}

std::vector<std::string> Resources::ListFiles() {
    if (m_PakHandler == nullptr) {
        std::cerr << "Resources::ListFiles: Pakker instance not set." << std::endl;
        return {};
    }

    return m_PakHandler->ListFiles(m_PakFile);
}

std::vector<std::string> Resources::ListFilesWithPrefix(const std::string &prefix) {
    if (m_PakHandler == nullptr) {
        std::cerr << "Resources::ListFilesWithPrefix: Pakker instance not set." << std::endl;
        return {};
    }

    return m_PakHandler->ListFilesWithPrefix(m_PakFile, prefix);
}
