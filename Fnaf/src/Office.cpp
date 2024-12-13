#include "Office.h"
#include "Core/Window.h"
#include "Graphics/LayerManager.h"
#include "Assets/Resources.h"
#include "Scene/SceneManager.h"
#include "Player.h"

void LeftBottomButtonCallback(bool active)
{
	std::cout << "Left Bottom Button Pressed" << std::endl;
}

void RightLightButtonCallback(bool active)
{
    std::cout << "Right Light Button Pressed" << std::endl;
}

Office::Office()
    : m_LeftDoorClosed(false)
    , m_RightDoorClosed(false)
    , m_LeftLightOn(false)
    , m_RightLightOn(false)
{
    m_OfficeTexture = Resources::GetTexture("Graphics/Office/NormalOffice.png");
    m_DoorTexture = Resources::GetTexture("Graphics/Office/door.png");

    std::vector<std::shared_ptr<sf::Texture>> LeftButtonsTextures;

    LeftButtonsTextures.push_back(Resources::GetTexture("Graphics/Office/ButtonsLeft/NoActive.png"));
    LeftButtonsTextures.push_back(Resources::GetTexture("Graphics/Office/ButtonsLeft/TopActive.png"));
    LeftButtonsTextures.push_back(Resources::GetTexture("Graphics/Office/ButtonsLeft/BothActive.png"));
    LeftButtonsTextures.push_back(Resources::GetTexture("Graphics/Office/ButtonsLeft/BottomActive.png"));
    m_LeftButtons.SetTextures(LeftButtonsTextures);

    std::vector<std::shared_ptr<sf::Texture>> RightButtonsTextures;

    RightButtonsTextures.push_back(Resources::GetTexture("Graphics/Office/ButtonsRight/NoActive.png"));
    RightButtonsTextures.push_back(Resources::GetTexture("Graphics/Office/ButtonsRight/TopActive.png"));
    RightButtonsTextures.push_back(Resources::GetTexture("Graphics/Office/ButtonsRight/BothActive.png"));
    RightButtonsTextures.push_back(Resources::GetTexture("Graphics/Office/ButtonsRight/BottomActive.png"));
    m_RightButtons.SetTextures(RightButtonsTextures);

    m_FreddyNoseButton.SetTexture(Resources::GetTexture("Graphics/ClickTeamFusion/1.png"));
    m_FreddyNoseButton.SetPosition(sf::Vector2f(667, 212.5));
    //m_FreddyNoseButton.SetLayer(3); // for debug

    m_LeftButtons.SetLayer(2);
    m_LeftButtons.SetCallbacks(
        [&](bool active) {
            ToggleLeftDoor();
            AudioManager::Instance().PlayDoorSound(m_LeftDoorClosed);
        },
        [&](bool active) {
            ToggleLeftLight();
        }
    );

    m_RightButtons.SetLayer(2);
    m_RightButtons.SetCallbacks(
        [&](bool active) {
            ToggleRightDoor();
            AudioManager::Instance().PlayDoorSound(m_RightDoorClosed);
        },
        [&](bool active) {
            ToggleRightLight();
        }
    );

    m_LeftDoor = FlipBook(2, 0.016f, false);
    m_RightDoor = FlipBook(2, 0.016f, false);
    for (int i = 1; i < 16; i++)
    {
        m_LeftDoor.AddFrame(Resources::GetTexture("Graphics/Office/LeftDoor/Frame" + std::to_string(i) + ".png"));
        m_RightDoor.AddFrame(Resources::GetTexture("Graphics/Office/RightDoor/Frame" + std::to_string(i) + ".png"));
    }

    m_FreddyNose = Resources::GetMusic("Audio/Office/PartyFavorraspyPart_AC01__3.wav");
}

void Office::Init()
{
    // Create sprites
    m_OfficeSprite = sf::Sprite(*m_OfficeTexture);

    LayerManager::AddDrawable(0, &m_OfficeSprite);

    // Set positions
    m_LeftButtons.SetPosition(-12.5, 250);
    m_RightButtons.SetPosition(1512.5, 250);
    m_LeftDoor.SetPosition(60, 0);
    m_RightDoor.SetPosition(1255, 0);

    auto FanEnt = SceneManager::GetActiveScene()->CreateEntity("Fan");
}

void Office::Update(double deltaTime)
{
    m_RightDoor.Update(deltaTime);
    m_RightDoor.RegisterToLayerManager();
	m_LeftDoor.Update(deltaTime);
	m_LeftDoor.RegisterToLayerManager();

    UpdateDoorStates();
    UpdateLightStates();
    UpdatePowerUsage();
}

void Office::FixedUpdate()
{
    if (player.m_UsingCamera)
        return;

    std::shared_ptr<sf::RenderWindow> window = Window::GetWindow();

    m_LeftButtons.updateButton(*window);
    m_RightButtons.updateButton(*window);

    if (m_FreddyNoseButton.IsClicked(*window))
    {
        // play click sound
        m_FreddyNose->play();
    }
}

void Office::Render()
{
}

void Office::Destroy()
{
}

void Office::ToggleLeftDoor() {
    m_LeftDoorClosed = !m_LeftDoorClosed;
    m_LeftDoor.Play(m_LeftDoorClosed);
    UpdatePowerUsage();
}

void Office::ToggleRightDoor() {
    m_RightDoorClosed = !m_RightDoorClosed;
    m_RightDoor.Play(m_RightDoorClosed);
    UpdatePowerUsage();
}

void Office::ToggleLeftLight() {
    m_LeftLightOn = !m_LeftLightOn;
    m_RightLightOn = false;  // Turn off other light if on
    UpdatePowerUsage();
}

void Office::ToggleRightLight() {
    m_RightLightOn = !m_RightLightOn;
    m_LeftLightOn = false;  // Turn off other light if on
    UpdatePowerUsage();
}

void Office::UpdatePowerUsage() {
    int usageLevel = 1;  // Base usage

    // Add usage for each active system
    if (m_LeftDoorClosed) usageLevel++;
    if (m_RightDoorClosed) usageLevel++;
    if (m_LeftLightOn) usageLevel++;
    if (m_RightLightOn) usageLevel++;

    // Update player's power usage level
    player.m_UsageLevel = std::min(usageLevel, MAX_POWER_USAGE);
}

void Office::UpdateDoorStates() {
    // Update door animations and states
    if (m_LeftDoorClosed) {
        player.m_UsingDoor = true;
    }
    if (m_RightDoorClosed) {
        player.m_UsingDoor = true;
    }
}

void Office::UpdateLightStates() {
    // Update light states
    player.m_UsingLight = m_LeftLightOn || m_RightLightOn;
}
