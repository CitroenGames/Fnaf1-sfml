#include "Resources.h"

#include <cstdint>
#include <iostream>
#include <map>
#include <memory>
#include <vector>

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>

#include "Audio/AudioClip.h"
#include "Pak.h"

namespace {
    std::string g_PakFile;
    Pakker *g_PakHandler = nullptr;

    std::map<std::string, std::shared_ptr<sf::Texture> > g_Textures;
    std::map<std::string, std::shared_ptr<AudioBuffer> > g_AudioBuffers;
    std::map<std::string, std::shared_ptr<sf::Font> > g_Fonts;
    std::map<std::string, std::shared_ptr<std::vector<uint8_t> > > g_FontBuffers;

    std::shared_ptr<std::vector<uint8_t> > LoadPakAsset(const std::string &filename,
                                                        const char *errorPrefix) {
        if (g_PakHandler == nullptr) {
            std::cerr << "Resources::Load: Pakker instance not set." << std::endl;
            return nullptr;
        }

        auto data = g_PakHandler->LoadFile(g_PakFile, filename);
        if (!data) {
            std::cerr << errorPrefix << filename << std::endl;
        }

        return data;
    }
}

void Resources::BindPakFile(const std::string &pakFilename) {
    if (g_PakHandler == nullptr) {
        std::cerr << "Resources::Load: Pakker instance not set." << std::endl;
        return;
    }
    g_PakFile = pakFilename;
}

void Resources::Unload() {
    g_Textures.clear();
    g_AudioBuffers.clear();
    g_Fonts.clear();
    g_FontBuffers.clear();
}

std::shared_ptr<sf::Texture> Resources::GetTexture(const std::string &filename) {
    auto textureIt = g_Textures.find(filename);
    if (textureIt == g_Textures.end()) {
        const auto textureData = LoadPakAsset(filename, "Resources::GetTexture: Failed to load texture: ");
        if (!textureData) {
            return nullptr;
        }

        auto texture = std::make_shared<sf::Texture>();
        if (!texture->loadFromMemory(textureData->data(), textureData->size())) {
            std::cerr << "Resources::GetTexture: Failed to load texture from memory: " << filename << std::endl;
            return nullptr;
        }

        textureIt = g_Textures.emplace(filename, texture).first;
    }

    return textureIt->second;
}

std::shared_ptr<AudioClip> Resources::GetMusic(const std::string &filename) {
    auto bufferIt = g_AudioBuffers.find(filename);
    if (bufferIt == g_AudioBuffers.end()) {
        const auto musicData = LoadPakAsset(filename, "Resources::GetMusic: Failed to load music: ");
        if (!musicData) {
            return nullptr;
        }

        auto audioBuffer = AudioBuffer::CreateFromMemory(musicData->data(), musicData->size(), filename);
        if (!audioBuffer) {
            return nullptr;
        }

        bufferIt = g_AudioBuffers.emplace(filename, std::move(audioBuffer)).first;
    }

    return AudioClip::Create(bufferIt->second, filename);
}

std::shared_ptr<sf::Font> Resources::GetFont(const std::string &filename) {
    auto fontIt = g_Fonts.find(filename);
    if (fontIt == g_Fonts.end()) {
        const auto fontData = LoadPakAsset(filename, "Resources::GetFont: Failed to load font: ");
        if (!fontData) {
            return nullptr;
        }

        auto font = std::make_shared<sf::Font>();
        if (!font->loadFromMemory(fontData->data(), fontData->size())) {
            std::cerr << "Resources::GetFont: Failed to load font from memory: " << filename << std::endl;
            return nullptr;
        }

        g_FontBuffers[filename] = fontData;
        fontIt = g_Fonts.emplace(filename, font).first;
    }

    return fontIt->second;
}

std::vector<std::string> Resources::ListFiles() {
    if (g_PakHandler == nullptr) {
        std::cerr << "Resources::ListFiles: Pakker instance not set." << std::endl;
        return {};
    }

    return g_PakHandler->ListFiles(g_PakFile);
}

std::vector<std::string> Resources::ListFilesWithPrefix(const std::string &prefix) {
    if (g_PakHandler == nullptr) {
        std::cerr << "Resources::ListFilesWithPrefix: Pakker instance not set." << std::endl;
        return {};
    }

    return g_PakHandler->ListFilesWithPrefix(g_PakFile, prefix);
}

void Resources::SetPakker(Pakker *pakker) {
    g_PakHandler = pakker;
}
