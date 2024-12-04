#include "Flipbook.h"

Flipbook::Flipbook() : m_Layer(m_Layer), m_FrameDuration(0), m_ElapsedTime(0.f), m_CurrentFrame(0), m_IsPlayingFlag(false), m_Loop(false) {}

Flipbook::Flipbook(int m_Layer, float m_FrameDuration, bool m_Loop)
    : m_Layer(m_Layer), m_FrameDuration(m_FrameDuration), m_ElapsedTime(0.f), m_CurrentFrame(0), m_IsPlayingFlag(false), m_Loop(m_Loop) {}

void Flipbook::AddFrame(std::shared_ptr<sf::Texture> texture) {
    m_Frames.push_back(std::make_shared<sf::Sprite>(*texture));
}

void Flipbook::AddFrame(std::shared_ptr<sf::Sprite> sprite) {
	m_Frames.push_back(sprite);
}

void Flipbook::AddFrames(const std::vector<std::shared_ptr<sf::Sprite>>& sprites)
{
	for (auto sprite : sprites)
	{
		m_Frames.push_back(sprite);
	}
}

void Flipbook::AddFrames(const std::vector<sf::Texture>& Textures)
{
	for (auto Texture : Textures)
	{
		m_Frames.push_back(std::make_shared<sf::Sprite>(Texture));
	}
}

void Flipbook::Update(float deltaTime) {
    if (!m_IsPlayingFlag || m_Frames.empty()) return;

    m_ElapsedTime += deltaTime;
    if (m_ElapsedTime >= m_FrameDuration) {
        m_ElapsedTime = 0.f;
        if (m_CurrentFrame + 1 < m_Frames.size()) {
            m_CurrentFrame++;
        }
        else if (m_Loop) {
            m_CurrentFrame = 0;  // Reset to the first frame if looping is enabled
        }
        else {
            m_IsPlayingFlag = false;  // Stop the animation if not looping
        }
    }
}

void Flipbook::RegisterToLayerManager() {
    if (!m_Frames.empty()) {
        LayerManager::RemoveDrawable(*m_Frames[m_CurrentFrame]);
        LayerManager::AddDrawable(m_Layer, *m_Frames[m_CurrentFrame]);
    }
}

void Flipbook::Play() {
    m_IsPlayingFlag = true;
}

void Flipbook::Pause() {
    m_IsPlayingFlag = false;
}

void Flipbook::Stop() {
    m_IsPlayingFlag = false;
    m_CurrentFrame = 0;
    m_ElapsedTime = 0.f;
}

void Flipbook::SetFrameDuration(float duration) {
    m_FrameDuration = duration;
}

void Flipbook::SetLoop(bool shouldLoop) {
    m_Loop = shouldLoop;
}

bool Flipbook::IsPlaying() const {
    return m_IsPlayingFlag;
}

