#pragma once
#include <SFML/Graphics.hpp>
#include "Composable.h"
#include "nlohmann/json.hpp"
#include "Animatronic.h"
#include "Animation/FlipBook.h"
#include <map>
#include <memory>

class CameraSystem : public Composable::Component {
public:
    enum class View {
        SHOW_STAGE,
        DINING_AREA,
        PIRATE_COVE,
        BACKSTAGE,
        SUPPLY_CLOSET,
        WEST_HALL,
        EAST_HALL,
        WEST_HALL_CORNER,
        EAST_HALL_CORNER,
        KITCHEN  // Audio only
    };

    CameraSystem();

    // Component Lifecycle Methods
    void Init();
    void Update(double deltaTime) override;
    void FixedUpdate() override {}

    // Camera Control Methods
    void SwitchView(View newView);
    void ToggleCamera();
    bool IsAnimatronicVisible(const Animatronic& animatronic) const;
    View GetCurrentView() const { return m_CurrentView; }
    bool IsHovering(const sf::Vector2i& mousePos) const;

    // Serialization (required by Component)
    nlohmann::json Serialize() const override { return nlohmann::json(); }
    void Deserialize(const nlohmann::json& data) override {}
    std::string GetTypeName() const override { return "CameraSystem"; }

private:
    View m_CurrentView;
    sf::Sprite m_ViewSprite;
    std::map<View, std::shared_ptr<sf::Texture>> m_ViewTextures;
    float m_TransitionTimer;
    bool m_IsTransitioning;

    // Camera flip animation
    std::unique_ptr<FlipBook> m_FlipAnimation;
    sf::Sound m_CameraSound;
    sf::Sprite m_CameraButton;
    bool m_IsAnimating;

    // Static mapping
    static std::map<Animatronic::Location, View> LocationToView;

    // Helper Methods
    void LoadViewTextures();
    void UpdateTransition(double deltaTime);
    void RenderCurrentView();
    bool IsValidView(View view) const;
    View GetViewForLocation(Animatronic::Location location) const;
    void InitializeFlipAnimation();
};
