#include "FlipBook.h"

FlipBook::FlipBook() : m_Layer(m_Layer), m_FrameDuration(0), m_ElapsedTime(0.f), m_CurrentFrame(0), m_IsPlayingFlag(false), m_Loop(false) {}

FlipBook::FlipBook(int m_Layer, float m_FrameDuration, bool m_Loop)
    : m_Layer(m_Layer), m_FrameDuration(m_FrameDuration), m_ElapsedTime(0.f), m_CurrentFrame(0), m_IsPlayingFlag(false), m_Loop(m_Loop) {}

void FlipBook::AddFrame(std::shared_ptr<sf::Texture> texture) {
    m_Frames.push_back(std::make_shared<sf::Sprite>(*texture));
}

void FlipBook::AddFrame(std::shared_ptr<sf::Sprite> sprite) {
	m_Frames.push_back(sprite);
}

void FlipBook::AddFrames(const std::vector<std::shared_ptr<sf::Sprite>>& sprites)
{
	for (auto sprite : sprites)
	{
		m_Frames.push_back(sprite);
	}
}

void FlipBook::AddFrames(const std::vector<sf::Texture>& Textures)
{
	for (auto Texture : Textures)
	{
		m_Frames.push_back(std::make_shared<sf::Sprite>(Texture));
	}
}

void FlipBook::Update(float deltaTime) {
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

void FlipBook::RegisterToLayerManager() {
    if (!m_Frames.empty()) {
        LayerManager::RemoveDrawable(*m_Frames[m_CurrentFrame]);
        LayerManager::AddDrawable(m_Layer, *m_Frames[m_CurrentFrame]);
    }
}

void FlipBook::Play() {
    m_IsPlayingFlag = true;
}

void FlipBook::Pause() {
    m_IsPlayingFlag = false;
}

void FlipBook::Stop() {
    m_IsPlayingFlag = false;
    m_CurrentFrame = 0;
    m_ElapsedTime = 0.f;
}

void FlipBook::SetFrameDuration(float duration) {
    m_FrameDuration = duration;
}

void FlipBook::SetLoop(bool shouldLoop) {
    m_Loop = shouldLoop;
}

bool FlipBook::IsPlaying() const {
    return m_IsPlayingFlag;
}

