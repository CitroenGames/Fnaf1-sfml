#include "Office.h"
#include "Core/Window.h"
#include "Graphics/LayerManager.h"
#include "Assets/Resources.h"
#include "Scene/SceneManager.h"
#include "GameState.h"
#include "LayerDefines.h"

void LeftBottomButtonCallback(bool active)
{
	std::cout << "Left Bottom Button Pressed" << std::endl;
}

void RightLightButtonCallback(bool active)
{
    std::cout << "Right Light Button Pressed" << std::endl;
}

Office::Office()
    : m_IsVisible(true)
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
    m_LeftButtons.SetCallbacks([&](bool active) { m_LeftDoor.Play(active); m_DoorNoise->play(); }, LeftBottomButtonCallback);

    m_RightButtons.SetLayer(2);
    m_RightButtons.SetCallbacks([&](bool active) { m_RightDoor.Play(active); m_DoorNoise->play(); }, RightLightButtonCallback);

    m_LeftDoor = FlipBook(2, 0.016f, false);
    m_RightDoor = FlipBook(2, 0.016f, false);
    for (int i = 1; i < 16; i++)
    {
        m_LeftDoor.AddFrame(Resources::GetTexture("Graphics/Office/LeftDoor/Frame" + std::to_string(i) + ".png"));
        m_RightDoor.AddFrame(Resources::GetTexture("Graphics/Office/RightDoor/Frame" + std::to_string(i) + ".png"));
    }

    m_FreddyNose = Resources::GetMusic("Audio/Office/PartyFavorraspyPart_AC01__3.wav");
    m_DoorNoise = Resources::GetMusic("Audio/Office/SFXBible_12478.wav");
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

void Office::HideOfficeElements()
{
    m_IsVisible = false;
    
    // Remove office sprite from layer manager
    LayerManager::RemoveDrawable(&m_OfficeSprite);
    
    // Remove buttons from layer manager
    m_LeftButtons.SetLayer(-1); // Use a non-visible layer
    m_RightButtons.SetLayer(-1);
    
    // Hide Freddy nose button
    LayerManager::RemoveDrawable(&m_FreddyNoseButton);
}

void Office::ShowOfficeElements()
{
    m_IsVisible = true;
    
    // Add office sprite back to layer manager
    LayerManager::AddDrawable(OFFICE_LAYER, &m_OfficeSprite);
    
    // Add buttons back to layer manager
    m_LeftButtons.SetLayer(BUTTON_LAYER);
    m_RightButtons.SetLayer(BUTTON_LAYER);
    
    // Show Freddy nose button
    LayerManager::AddDrawable(BUTTON_LAYER, &m_FreddyNoseButton);
}

void Office::Update(double deltaTime)
{
    m_RightDoor.Update(deltaTime);
    m_RightDoor.RegisterToLayerManager();
	m_LeftDoor.Update(deltaTime);
	m_LeftDoor.RegisterToLayerManager();
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
