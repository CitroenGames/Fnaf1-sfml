#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <Graphics/LayerManager.h>

class GlitchEffect {
private:
    std::vector<std::shared_ptr<sf::Sprite>> m_Frames;
    int m_Layer;
    int m_CurrentFrame;

    // Glitch effect parameters
    bool m_IsGlitching;
    float m_GlitchTimer;
    float m_GlitchChance;        // Chance to start a glitch
    float m_MultiGlitchChance;   // Chance for multiple random frames
    float m_GlitchDuration;      // How long each glitch frame shows
    std::vector<int> m_GlitchSequence;  // Sequence of frames to show during glitch

public:
    GlitchEffect()
        : m_Layer(0)
        , m_CurrentFrame(0)
        , m_IsGlitching(false)
        , m_GlitchTimer(0.0f)
        , m_GlitchChance(0.001f)
        , m_MultiGlitchChance(0.3f)
        , m_GlitchDuration(0.05f)
    {
    }

    GlitchEffect(int layer)
        : m_Layer(layer)
        , m_CurrentFrame(0)
        , m_IsGlitching(false)
        , m_GlitchTimer(0.0f)
        , m_GlitchChance(0.001f)
        , m_MultiGlitchChance(0.3f)
        , m_GlitchDuration(0.05f)
    {
    }

    ~GlitchEffect() {
        // Clean up current frame from LayerManager
        if (!m_Frames.empty()) {
            LayerManager::RemoveDrawable(m_Frames[m_CurrentFrame].get());
        }
    }

    void AddFrame(std::shared_ptr<sf::Texture> texture) {
        m_Frames.push_back(std::make_shared<sf::Sprite>(*texture));
        if (m_Frames.size() == 1) {  // First frame added
            RegisterToLayerManager();
        }
    }

    void AddFrame(std::shared_ptr<sf::Sprite> sprite) {
        m_Frames.push_back(sprite);
        if (m_Frames.size() == 1) {  // First frame added
            RegisterToLayerManager();
        }
    }

    void AddFrames(const std::vector<std::shared_ptr<sf::Sprite>>& sprites) {
        bool wasEmpty = m_Frames.empty();
        for (const auto& sprite : sprites) {
            m_Frames.push_back(sprite);
        }
        if (wasEmpty && !m_Frames.empty()) {  // First frames added
            RegisterToLayerManager();
        }
    }

    void AddFrames(const std::vector<std::shared_ptr<sf::Texture>>& textures) {
        bool wasEmpty = m_Frames.empty();
        for (const auto& texture : textures) {
            m_Frames.push_back(std::make_shared<sf::Sprite>(*texture));
        }
        if (wasEmpty && !m_Frames.empty()) {  // First frames added
            RegisterToLayerManager();
        }
    }

    void SetPosition(float x, float y) {
        for (auto& sprite : m_Frames) {
            sprite->setPosition(x, y);
        }
    }

    void SetLayer(int layer) {
        if (m_Layer != layer) {
            if (!m_Frames.empty()) {
                LayerManager::ChangeLayer(m_Frames[m_CurrentFrame].get(), layer);
            }
            m_Layer = layer;
        }
    }

    void Update() {
        if (m_Frames.empty()) return;

        if (m_IsGlitching) {
            m_GlitchTimer += 1.0f / 66.0f;  // Fixed timestep for 66 tickrate
            if (m_GlitchTimer >= m_GlitchDuration) {
                if (m_GlitchSequence.empty()) {
                    // End of glitch sequence, return to normal
                    m_IsGlitching = false;
                    m_CurrentFrame = 0;  // Return to first frame
                }
                else {
                    // Show next frame in glitch sequence
                    m_CurrentFrame = m_GlitchSequence.back();
                    m_GlitchSequence.pop_back();
                    m_GlitchTimer = 0.0f;
                }
                RegisterToLayerManager();
            }
        }
        else {
            // Normal state - show first frame and check for glitch trigger
            if (static_cast<float>(rand()) / RAND_MAX < m_GlitchChance) {
                StartGlitch();
            }
        }
    }

private:
    void RegisterToLayerManager() {
        if (!m_Frames.empty()) {
            LayerManager::RemoveDrawable(m_Frames[m_CurrentFrame].get());
            LayerManager::AddDrawable(m_Layer, m_Frames[m_CurrentFrame].get());
        }
    }

public:
    void StartGlitch() {
        m_IsGlitching = true;
        m_GlitchTimer = 0.0f;
        m_GlitchSequence.clear();

        // Always add at least one random frame
        int numGlitchFrames = 1;

        // Chance to add more frames
        while (static_cast<float>(rand()) / RAND_MAX < m_MultiGlitchChance && numGlitchFrames < 4) {
            numGlitchFrames++;
        }

        // Generate random frame sequence
        for (int i = 0; i < numGlitchFrames; i++) {
            int randomFrame;
            do {
                randomFrame = rand() % m_Frames.size();
            } while (randomFrame == 0);  // Don't include the normal frame
            m_GlitchSequence.push_back(randomFrame);
        }

        // Start showing the first glitch frame
        if (!m_GlitchSequence.empty()) {
            m_CurrentFrame = m_GlitchSequence.back();
            m_GlitchSequence.pop_back();
            RegisterToLayerManager();
        }
    }

    void SetGlitchParameters(float chance, float multiChance, float duration) {
        m_GlitchChance = chance;
        m_MultiGlitchChance = multiChance;
        m_GlitchDuration = duration;
    }
};