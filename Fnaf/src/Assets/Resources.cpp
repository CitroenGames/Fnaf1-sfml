#include "Resources.h"

std::string Resources::pakFile;
Pakker Resources::pakHandler;
std::map<std::string, std::shared_ptr<sf::Texture>> Resources::textures;
std::map<std::string, std::shared_ptr<sf::SoundBuffer>> Resources::soundBuffers;
std::map<std::string, std::shared_ptr<std::vector<uint8_t>>> Resources::musicBuffers;

void Resources::Load(const std::string& pakFilename) {
    pakFile = pakFilename;
}

void Resources::Unload() {
    textures.clear();
    soundBuffers.clear();
    musicBuffers.clear();
}

std::shared_ptr<sf::Texture> Resources::GetTexture(const std::string& filename) {
    if (textures.find(filename) == textures.end()) {
        auto textureData = pakHandler.loadFile(pakFile, filename);
        if (!textureData) {
            return nullptr;
        }

        std::shared_ptr<sf::Texture> texture = std::make_shared<sf::Texture>();
        if (!texture->loadFromMemory(textureData->data(), textureData->size())) {
            return nullptr;
        }

        textures[filename] = texture;
    }

    return textures[filename];
}

std::shared_ptr<sf::SoundBuffer> Resources::GetSoundBuffer(const std::string& filename) {
    if (soundBuffers.find(filename) == soundBuffers.end()) {
        auto soundData = pakHandler.loadFile(pakFile, filename);
        if (!soundData) {
            return nullptr;
        }

        std::shared_ptr<sf::SoundBuffer> buffer = std::make_shared<sf::SoundBuffer>();
        if (!buffer->loadFromMemory(soundData->data(), soundData->size())) {
            return nullptr;
        }

        soundBuffers[filename] = buffer;
    }

    return soundBuffers[filename];
}

std::shared_ptr<sf::Music> Resources::GetMusic(const std::string& filename) {
    if (musicBuffers.find(filename) == musicBuffers.end()) {
        auto musicData = pakHandler.loadFile(pakFile, filename);
        if (!musicData) {
            return nullptr;
        }

        // Store the buffer to ensure its memory persists
        musicBuffers[filename] = musicData;
    }

    // Create and initialize sf::Music from memory
    std::shared_ptr<sf::Music> music = std::make_shared<sf::Music>();
    if (!music->openFromMemory(musicBuffers[filename]->data(), musicBuffers[filename]->size())) {
        return nullptr;
    }

    return music;
}
