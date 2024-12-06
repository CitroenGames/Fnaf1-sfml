#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "Graphics/LayerManager.h"
#include <memory>

class FlipBook {
public:
    FlipBook();
    FlipBook(int layer, float frameDuration = 0.1f, bool loop = true);

    // Add a new frame to the flipbook
    void AddFrame(std::shared_ptr<sf::Texture> texture);
    void AddFrame(std::shared_ptr<sf::Sprite> sprite);

    void AddFrames(const std::vector<std::shared_ptr<sf::Sprite>>& sprites);
	void AddFrames(const std::vector<sf::Texture>& textures);

    void Update(float deltaTime);

    // Register the current frame to the LayerManager
    void RegisterToLayerManager();

    void Play();
    void Pause();
    void Stop();
    void SetFrameDuration(float duration);
    void SetLoop(bool shouldLoop);

    bool IsPlaying() const;

    void SetPosition(float x, float y) {
        for (auto& frame : m_Frames) {
            frame->setPosition(x, y);
        }
    }

    void Cleanup() {
        for (const auto& frame : m_Frames) {
            LayerManager::RemoveDrawable(*frame);
        }
    }

private:
    std::vector<std::shared_ptr<sf::Sprite>> m_Frames;
    float m_FrameDuration;
    float m_ElapsedTime;
    std::size_t m_CurrentFrame;
    bool m_IsPlayingFlag;
    bool m_Loop;  // Whether the animation should loop
    int m_Layer;  // The layer to which this flipbook's frames will be added
};
