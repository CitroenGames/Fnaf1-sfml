#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "Layers/LayerManager.h"

class Flipbook {
public:
    Flipbook();
    Flipbook(int layer, float frameDuration = 0.1f, bool loop = true);

    // Add a new frame to the flipbook
    void addFrame(const sf::Texture& texture);
    void addFrame(const sf::Sprite& sprite);

    void addFrames(const std::vector<sf::Sprite>& sprites);
	void addFrames(const std::vector<sf::Texture>& textures);

    // Update the flipbook (to be called every frame)
    void update(float deltaTime);

    // Register the current frame to the LayerManager
    void registerToLayerManager();

    // Start playing the flipbook
    void play();

    // Pause the flipbook
    void pause();

    // Stop the flipbook and reset to the first frame
    void stop();

    // Set the speed of the animation
    void setFrameDuration(float duration);

    // Set whether the animation should loop
    void setLoop(bool shouldLoop);

    // Check if the flipbook is currently playing
    bool isPlaying() const;

    void setPosition(float x, float y) {
        for (auto& frame : frames) {
            frame.setPosition(x, y);
        }
    }

    void Destroy() {
        for (const auto& frame : frames) {
            LayerManager::RemoveDrawable(frame);
        }
    }

private:
    std::vector<sf::Sprite> frames;
    float frameDuration;
    float elapsedTime;
    std::size_t currentFrame;
    bool isPlayingFlag;
    bool loop;  // Whether the animation should loop
    int layer;  // The layer to which this flipbook's frames will be added
};
