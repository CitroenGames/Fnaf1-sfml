#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <memory>
#include <map>
#include "UI/ImageButton.h"
#include "fnaf.hpp"

class CameraSystem {
public:
    CameraSystem();
    ~CameraSystem();

    void Init();
    void Update(float deltaTime);
    void Render(sf::RenderTarget& target);
    void ToggleCamera();
    void SwitchToRoom(Room newRoom);
    void SetStaticIntensity(float intensity);

    bool IsActive() const { return m_IsActive; }
    Room GetCurrentRoom() const { return m_CurrentRoom; }

private:
    Room m_CurrentRoom;
    Room m_PreviousRoom;
    bool m_IsActive;
    bool m_IsTransitioning;
    bool m_IsTogglingCamera;
    float m_ToggleProgress;
    std::unique_ptr<ImageButton> m_CameraButton;
    std::map<Room, sf::Sprite> m_CameraViews;
    std::map<Room, std::unique_ptr<ImageButton>> m_CameraButtons;
    sf::Sprite m_StaticEffect;
    sf::Sprite m_WhiteEffect;
    sf::Sprite m_MapSprite;
    sf::Sound m_CameraSound;
    float m_TransitionProgress;
    float m_StaticIntensity;
    float m_StaticTimer;
    float m_WhiteTimer;
    int m_CurrentStaticFrame;
    int m_CurrentWhiteFrame;

    static constexpr float TRANSITION_SPEED = 2.0f;
    static constexpr float TOGGLE_SPEED = 3.0f;
    static constexpr float STATIC_FRAME_DURATION = 0.05f;
    static constexpr float WHITE_FRAME_DURATION = 0.1f;

    void UpdateStaticEffect();
    void InitializeMap();
    std::unique_ptr<ImageButton> CreateCameraButton(const sf::Vector2f& position);
};
