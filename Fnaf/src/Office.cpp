#include "Office.h"
#include "Core/Window.h"
#include "Graphics/LayerManager.h"
#include "Assets/Resources.h"
#include "Scene/SceneManager.h"
#include "GameState.h"
#include "LayerDefines.h"

Office::Office()
    : m_IsVisible(true)
      , m_GameRef(nullptr) {
    m_OfficeTexture = Resources::GetTexture("Graphics/Office/NormalOffice.png");
    m_DoorTexture = Resources::GetTexture("Graphics/Office/door.png");
    m_PowerOutageTexture = Resources::GetTexture("Graphics/Office/Office_NoPower1.png");
    m_PowerOutageTexture2 = Resources::GetTexture("Graphics/Office/Office_NoPower2.png");

    // Load light textures
    m_LeftLightTexture = Resources::GetTexture("Graphics/Office/Light/Office_LightLeft.png");
    m_RightLightTexture = Resources::GetTexture("Graphics/Office/Light/Office_LightRight.png");
    m_LeftLightBonnieTexture = Resources::GetTexture("Graphics/Office/Light/Office_LightBonnie.png");
    m_RightLightChicaTexture = Resources::GetTexture("Graphics/Office/Light/Office_LightChicka.png");

    {
        std::vector<std::shared_ptr<sf::Texture> > LeftButtonsTextures;

        LeftButtonsTextures.push_back(Resources::GetTexture("Graphics/Office/ButtonsLeft/NoActive.png"));
        LeftButtonsTextures.push_back(Resources::GetTexture("Graphics/Office/ButtonsLeft/TopActive.png"));
        LeftButtonsTextures.push_back(Resources::GetTexture("Graphics/Office/ButtonsLeft/BothActive.png"));
        LeftButtonsTextures.push_back(Resources::GetTexture("Graphics/Office/ButtonsLeft/BottomActive.png"));
        m_LeftButtons.SetTextures(LeftButtonsTextures);

        std::vector<std::shared_ptr<sf::Texture> > RightButtonsTextures;

        RightButtonsTextures.push_back(Resources::GetTexture("Graphics/Office/ButtonsRight/NoActive.png"));
        RightButtonsTextures.push_back(Resources::GetTexture("Graphics/Office/ButtonsRight/TopActive.png"));
        RightButtonsTextures.push_back(Resources::GetTexture("Graphics/Office/ButtonsRight/BothActive.png"));
        RightButtonsTextures.push_back(Resources::GetTexture("Graphics/Office/ButtonsRight/BottomActive.png"));
        m_RightButtons.SetTextures(RightButtonsTextures);
    }

    m_FreddyNoseButton.SetTexture(Resources::GetTexture("Graphics/ClickTeamFusion/1.png"));
    m_FreddyNoseButton.SetPosition(sf::Vector2f(667, 212.5));
    // Remove from LayerManager — nose button is an invisible clickable hitbox, not a visible sprite
    LayerManager::RemoveDrawable(&m_FreddyNoseButton);

    m_LeftButtons.SetLayer(2);
    m_LeftButtons.SetCallbacks(
        [&](bool active) {
            // Door callback
            player.m_LeftDoorClosed = active;
            player.UpdateUsageLevel();

            m_LeftDoor.Play(active);
            m_DoorNoise->play();
        },
        [&](bool active) {
            // Light callback
            ToggleLeftLight(active);
        }
    );

    m_RightButtons.SetLayer(2);
    m_RightButtons.SetCallbacks(
        [&](bool active) {
            // Door callback
            player.m_RightDoorClosed = active;
            player.UpdateUsageLevel();

            m_RightDoor.Play(active);
            m_DoorNoise->play();
        },
        [&](bool active) {
            // Light callback
            ToggleRightLight(active);
        }
    );

    m_LeftDoor = FlipBook(2, 0.016f, false);
    m_RightDoor = FlipBook(2, 0.016f, false);
    for (int i = 1; i <= 16; i++) {
        m_LeftDoor.AddFrame(Resources::GetTexture("Graphics/Office/LeftDoor/Frame" + std::to_string(i) + ".png"));
        m_RightDoor.AddFrame(Resources::GetTexture("Graphics/Office/RightDoor/Frame" + std::to_string(i) + ".png"));
    }

    m_FreddyNose = Resources::GetMusic("Audio/Office/PartyFavorraspyPart_AC01__3.wav");
    m_DoorNoise = Resources::GetMusic("Audio/Office/SFXBible_12478.wav");
    m_LightSound = Resources::GetMusic("Audio/Office/BallastHumMedium2.wav");
}

void Office::Init() {
    // Create sprites
    m_OfficeSprite = sf::Sprite(*m_OfficeTexture);

    LayerManager::AddDrawable(0, &m_OfficeSprite);

    // Set positions
    m_LeftButtons.SetPosition(-12.5, 250);
    m_RightButtons.SetPosition(1512.5, 250);
    m_LeftDoor.SetPosition(60, 0);
    m_RightDoor.SetPosition(1255, 0);

    // Configure light sound for looping
    m_LightSound->setLoop(true);
    m_LightSound->setVolume(50.0f);

    // Subscribe to power outage event
    GameEvents::Subscribe(GameEvent::POWER_OUTAGE, [this]() {
        HandlePowerOutage();
    });
}

void Office::HandlePowerOutage() {
    m_PowerOutage = true;

    // Switch to power outage texture
    m_OfficeSprite.setTexture(*m_PowerOutageTexture);

    // Turn off any active lights and update player state
    if (player.m_LeftLightOn) {
        m_LeftButtons.updateBottomState(false);
        player.m_LeftLightOn = false;
    }
    if (player.m_RightLightOn) {
        m_RightButtons.updateBottomState(false);
        player.m_RightLightOn = false;
    }

    // Reset door animations — doors open when power dies
    m_LeftDoor.Stop();
    m_RightDoor.Stop();

    // Update usage level
    player.UpdateUsageLevel();

    // Stop light sound if playing
    if (m_LightSound->getStatus() == sf::Music::Playing) {
        m_LightSound->stop();
    }

    // Disable door controls during power outage
    m_LeftButtons.SetEnabled(false);
    m_RightButtons.SetEnabled(false);
}

void Office::HideOfficeElements() {
    m_IsVisible = false;

    // Remove office sprite from layer manager
    LayerManager::RemoveDrawable(&m_OfficeSprite);

    // Remove buttons from layer manager
    m_LeftButtons.SetLayer(-1); // Use a non-visible layer
    m_RightButtons.SetLayer(-1);

    // Hide door FlipBook frames
    m_LeftDoor.UnregisterFromLayerManager();
    m_RightDoor.UnregisterFromLayerManager();
}

void Office::ShowOfficeElements() {
    m_IsVisible = true;

    // Add office sprite back to layer manager
    LayerManager::AddDrawable(OFFICE_LAYER, &m_OfficeSprite);

    // Add buttons back to layer manager
    m_LeftButtons.SetLayer(BUTTON_LAYER);
    m_RightButtons.SetLayer(BUTTON_LAYER);

    // Re-register door FlipBooks if doors are closed
    if (player.m_LeftDoorClosed) {
        m_LeftDoor.RegisterToLayerManager();
    }
    if (player.m_RightDoorClosed) {
        m_RightDoor.RegisterToLayerManager();
    }
}

void Office::Update(double deltaTime) {
    m_RightDoor.Update(deltaTime);
    m_LeftDoor.Update(deltaTime);

    // Flicker Freddy's face during power outage FREDDY_FACE phase
    if (m_PowerOutage && m_GameRef) {
        auto phase = m_GameRef->GetPowerOutagePhase();

        if (phase == PowerOutagePhase::FREDDY_FACE) {
            m_FlickerTimer += static_cast<float>(deltaTime);
            if (m_FlickerTimer >= 0.25f) {
                m_FlickerTimer = 0.0f;
                m_FlickerState = !m_FlickerState;
                m_OfficeSprite.setTexture(
                    m_FlickerState ? *m_PowerOutageTexture2 : *m_PowerOutageTexture);
            }
        }
        else if (phase == PowerOutagePhase::LIGHTS_OFF || phase == PowerOutagePhase::JUMPSCARE) {
            m_OfficeSprite.setTexture(*m_PowerOutageTexture);
        }
    }
}

void Office::FixedUpdate() {
    if (player.m_UsingCamera)
        return;

    const auto window = Window::GetWindow();

    m_LeftButtons.updateButton(*window);
    m_RightButtons.updateButton(*window);

    if (m_FreddyNoseButton.IsClicked(*window)) {
        // play click sound
        m_FreddyNose->play();
    }
}

void Office::ToggleLeftLight(bool active) {
    // If right light is on and we're activating left light, turn off right light first
    if (active && player.m_RightLightOn) {
        // Update right button state
        m_RightButtons.updateBottomState(false);
        player.m_RightLightOn = false;
    }

    player.m_LeftLightOn = active;
    player.UpdateUsageLevel();

    if (active) {
        // Check if Bonnie is at the door and use appropriate texture
        bool bonnieAtDoor = false;
        if (m_GameRef) {
            const Room bonnieLocation = m_GameRef->GetAnimatronicLocation("Bonnie");
            bonnieAtDoor = (bonnieLocation == Room::WEST_CORNER);
        }

        if (bonnieAtDoor) {
            m_OfficeSprite.setTexture(*m_LeftLightBonnieTexture);
        } else {
            m_OfficeSprite.setTexture(*m_LeftLightTexture);
        }

        // Start looping light sound
        if (m_LightSound->getStatus() != sf::Music::Playing) {
            m_LightSound->play();
        }
    } else {
        // Reset office texture if not in power outage
        if (m_GameRef && !m_GameRef->IsPowerOutage()) {
            m_OfficeSprite.setTexture(*m_OfficeTexture);
        }

        // If no lights are active, stop the sound
        if (!player.m_RightLightOn) {
            m_LightSound->stop();
        }
    }
}

void Office::ToggleRightLight(bool active) {
    // If left light is on and we're activating right light, turn off left light first
    if (active && player.m_LeftLightOn) {
        // Update left button state
        m_LeftButtons.updateBottomState(false);
        player.m_LeftLightOn = false;
    }

    player.m_RightLightOn = active;
    player.UpdateUsageLevel();

    if (active) {
        // Check if Chica is at the door and use appropriate texture
        bool chicaAtDoor = false;
        if (m_GameRef) {
            Room chicaLocation = m_GameRef->GetAnimatronicLocation("Chica");
            chicaAtDoor = (chicaLocation == Room::EAST_CORNER);
        }

        if (chicaAtDoor) {
            m_OfficeSprite.setTexture(*m_RightLightChicaTexture);
        } else {
            m_OfficeSprite.setTexture(*m_RightLightTexture);
        }

        // Start looping light sound
        if (m_LightSound->getStatus() != sf::Music::Playing) {
            m_LightSound->play();
        }
    } else {
        // Reset office texture if not in power outage
        if (m_GameRef && !m_GameRef->IsPowerOutage()) {
            m_OfficeSprite.setTexture(*m_OfficeTexture);
        }

        // If no lights are active, stop the sound
        if (!player.m_LeftLightOn) {
            m_LightSound->stop();
        }
    }
}
