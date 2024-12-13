#include "CameraSystem.h"
#include "Assets/Resources.h"
#include "Graphics/LayerManager.h"
#include "Player.h"
#include "LayerDefines.h"
#include "AudioManager.h"

// Initialize static location to view mapping
std::map<Animatronic::Location, CameraSystem::View> CameraSystem::LocationToView = {
    {Animatronic::Location::SHOW_STAGE, View::SHOW_STAGE},
    {Animatronic::Location::DINING_AREA, View::DINING_AREA},
    {Animatronic::Location::PIRATE_COVE, View::PIRATE_COVE},
    {Animatronic::Location::BACKSTAGE, View::BACKSTAGE},
    {Animatronic::Location::SUPPLY_CLOSET, View::SUPPLY_CLOSET},
    {Animatronic::Location::WEST_HALL, View::WEST_HALL},
    {Animatronic::Location::EAST_HALL, View::EAST_HALL},
    {Animatronic::Location::WEST_HALL_CORNER, View::WEST_HALL_CORNER},
    {Animatronic::Location::EAST_HALL_CORNER, View::EAST_HALL_CORNER},
    {Animatronic::Location::KITCHEN, View::KITCHEN}
};

CameraSystem::CameraSystem()
    : m_CurrentView(View::SHOW_STAGE)
    , m_TransitionTimer(0.0f)
    , m_IsTransitioning(false)
{
}

void CameraSystem::Init() {
    LoadViewTextures();

    // Set up initial view
    if (m_ViewTextures.count(m_CurrentView) > 0) {
        m_ViewSprite.setTexture(*m_ViewTextures[m_CurrentView]);
    }

    // Add sprite to render layer
    LayerManager::AddDrawable(CAMERA_LAYER, &m_ViewSprite);
}

void CameraSystem::Update(double deltaTime) {
    if (m_IsTransitioning) {
        UpdateTransition(deltaTime);
    }

    if (player.m_UsingCamera) {
        RenderCurrentView();
        AudioManager::Instance().PlayAmbient(m_CurrentView);
    }
}

void CameraSystem::SwitchView(View newView) {
    if (!IsValidView(newView) || newView == m_CurrentView) {
        return;
    }

    m_CurrentView = newView;
    m_IsTransitioning = true;
    m_TransitionTimer = 0.0f;

    // Update sprite texture
    if (m_ViewTextures.count(m_CurrentView) > 0) {
        m_ViewSprite.setTexture(*m_ViewTextures[m_CurrentView]);
    }
}

void CameraSystem::ToggleCamera() {
    player.m_UsingCamera = !player.m_UsingCamera;
    if (player.m_UsingCamera) {
        // Increase power usage when camera is active
        player.m_UsageLevel = std::min(player.m_UsageLevel + 1, 5);
    } else {
        // Decrease power usage when camera is inactive
        player.m_UsageLevel = std::max(player.m_UsageLevel - 1, 1);
    }
}

bool CameraSystem::IsAnimatronicVisible(const Animatronic& animatronic) const {
    // Can't see animatronics if not using camera
    if (!player.m_UsingCamera) {
        return false;
    }

    // Special case for Kitchen (audio only)
    if (animatronic.GetLocation() == Animatronic::Location::KITCHEN) {
        return false;
    }

    // Check if animatronic is in current view
    return GetViewForLocation(animatronic.GetLocation()) == m_CurrentView;
}

void CameraSystem::LoadViewTextures() {
    // Load textures for each camera view
    m_ViewTextures[View::SHOW_STAGE] = Resources::GetTexture("Graphics/Cameras/ShowStage.png");
    m_ViewTextures[View::DINING_AREA] = Resources::GetTexture("Graphics/Cameras/DiningArea.png");
    m_ViewTextures[View::PIRATE_COVE] = Resources::GetTexture("Graphics/Cameras/PirateCove.png");
    m_ViewTextures[View::BACKSTAGE] = Resources::GetTexture("Graphics/Cameras/Backstage.png");
    m_ViewTextures[View::SUPPLY_CLOSET] = Resources::GetTexture("Graphics/Cameras/SupplyCloset.png");
    m_ViewTextures[View::WEST_HALL] = Resources::GetTexture("Graphics/Cameras/WestHall.png");
    m_ViewTextures[View::EAST_HALL] = Resources::GetTexture("Graphics/Cameras/EastHall.png");
    m_ViewTextures[View::WEST_HALL_CORNER] = Resources::GetTexture("Graphics/Cameras/WestHallCorner.png");
    m_ViewTextures[View::EAST_HALL_CORNER] = Resources::GetTexture("Graphics/Cameras/EastHallCorner.png");
}

void CameraSystem::UpdateTransition(double deltaTime) {
    m_TransitionTimer += deltaTime;

    // Transition duration: 0.2 seconds
    if (m_TransitionTimer >= 0.2f) {
        m_IsTransitioning = false;
        m_TransitionTimer = 0.0f;
    }
}

void CameraSystem::RenderCurrentView() {
    // Only render if we have a valid texture
    if (m_ViewTextures.count(m_CurrentView) > 0) {
        // Apply transition effect if transitioning
        if (m_IsTransitioning) {
            float alpha = std::min(1.0f, m_TransitionTimer / 0.2f);
            sf::Color color = m_ViewSprite.getColor();
            color.a = static_cast<sf::Uint8>(255.0f * alpha);
            m_ViewSprite.setColor(color);
        } else {
            m_ViewSprite.setColor(sf::Color::White);
        }
    }
}

bool CameraSystem::IsValidView(View view) const {
    return m_ViewTextures.count(view) > 0;
}

CameraSystem::View CameraSystem::GetViewForLocation(Animatronic::Location location) const {
    auto it = LocationToView.find(location);
    if (it != LocationToView.end()) {
        return it->second;
    }
    return m_CurrentView; // Return current view if location mapping not found
}
