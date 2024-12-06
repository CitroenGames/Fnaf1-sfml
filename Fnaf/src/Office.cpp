#include "Office.h"
#include "Core/Window.h"
#include "Graphics/LayerManager.h"
#include "Assets/Resources.h"
#include "Scene/SceneManager.h"
#include "Player.h"

void LeftTopButtonCallback(bool active)
{
	std::cout << "Left Top Button Pressed" << std::endl;
}

void LeftBottomButtonCallback(bool active)
{
	std::cout << "Left Bottom Button Pressed" << std::endl;
}

void RightDoorButtonCallback(bool active)
{
    std::cout << "Right Door Button Pressed" << std::endl;
}

void RightLightButtonCallback(bool active)
{
    std::cout << "Right Light Button Pressed" << std::endl;
}

Office::Office()
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
    m_LeftButtons.SetCallbacks(LeftTopButtonCallback, LeftBottomButtonCallback);

    m_RightButtons.SetLayer(2);
    m_RightButtons.SetCallbacks(RightDoorButtonCallback, RightLightButtonCallback);

    m_FreddyNose = Resources::GetMusic("Audio/Office/PartyFavorraspyPart_AC01__3.wav");
    m_FreddyNose->stop();
}

void Office::Init()
{
    // Create sprites
    m_OfficeSprite = sf::Sprite(*m_OfficeTexture);
    m_LeftDoorSprite = sf::Sprite(*m_DoorTexture);
    m_RightDoorSprite = sf::Sprite(*m_DoorTexture);

    LayerManager::AddDrawable(0, m_OfficeSprite);
    LayerManager::AddDrawable(1, m_LeftDoorSprite);
    LayerManager::AddDrawable(1, m_RightDoorSprite);

    // Initial positions
    m_LeftButtons.SetPosition(-12.5, 250);
    m_RightButtons.SetPosition(1512.5, 250); 
    m_LeftDoorSprite.setPosition(100, 500);    
    m_RightDoorSprite.setPosition(650, 500);  

    auto FanEnt = SceneManager::GetActiveScene()->CreateEntity("Fan");
}

void Office::Update(double deltaTime)
{
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
