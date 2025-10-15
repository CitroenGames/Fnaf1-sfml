#pragma once

class FlipBook {
public:
    FlipBook();
    ~FlipBook();
    FlipBook(int layer, float frameDuration = 0.1f, bool loop = true);

    // Add frames to the flipbook
    void AddFrame(std::shared_ptr<sf::Texture> texture);
    void AddFrame(std::shared_ptr<sf::Sprite> sprite);
    void AddFrames(const std::vector<std::shared_ptr<sf::Sprite>>& sprites);
    void AddFrames(const std::vector<sf::Texture>& textures);

    // Animation control
    void Update(float deltaTime);
    void Play(bool forward = true);
    void Pause();
    void Stop();
    void Cleanup();

    // Layer management
    void RegisterToLayerManager();
    void UnregisterFromLayerManager();

    // Setters
    void SetPosition(float x, float y);
    void SetFrameDuration(float duration);
    void SetLoop(bool shouldLoop);

    // Getters
    bool IsPlaying() const;
    sf::Sprite* GetCurrentFrame();
    const sf::Sprite* GetCurrentFrame() const;

private:
    std::vector<std::shared_ptr<sf::Sprite>> m_Frames;
    float m_FrameDuration;
    float m_ElapsedTime;
    std::size_t m_CurrentFrame;
    bool m_IsPlayingFlag;
    bool m_Loop;
    int m_Layer;
    bool m_IsForward;

    void UpdateLayerManagerRegistration();
};