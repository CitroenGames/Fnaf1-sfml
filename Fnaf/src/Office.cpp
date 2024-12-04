#include "Office.h"
#include "Core/Window.h"
#include "Graphics/LayerManager.h"
#include "Assets/Resources.h"
#include "Scene/SceneManager.h"

void LeftTopButtonCallback(bool active)
{
	std::cout << "Left Top Button Pressed" << std::endl;
}

void LeftBottomButtonCallback(bool active)
{
	std::cout << "Left Bottom Button Pressed" << std::endl;
}

void RightTopButtonCallback(bool active)
{
    std::cout << "Right Top Button Pressed" << std::endl;
}

void RightBottomButtonCallback(bool active)
{
    std::cout << "Right Bottom Button Pressed" << std::endl;
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

    m_LeftButtons.SetLayer(2);
    m_LeftButtons.SetCallbacks(LeftTopButtonCallback, LeftBottomButtonCallback);


    m_RightButtons.SetLayer(2);
    m_RightButtons.SetCallbacks(RightTopButtonCallback, RightBottomButtonCallback);
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

    // Scale the office sprite
    m_OfficeSprite.setScale(0.9f, 0.9f);

    // Initial positions
    m_LeftButtons.SetPosition(0, 250);
    m_RightButtons.SetPosition(1350, 250); 
    m_LeftDoorSprite.setPosition(100, 500);    
    m_RightDoorSprite.setPosition(650, 500);  

    auto FanEnt = SceneManager::GetActiveScene()->CreateEntity("Fan");
}

void Office::Update(double deltaTime)
{
}

void Office::FixedUpdate()
{
    std::shared_ptr<sf::RenderWindow> window = Window::GetWindow();
    m_LeftButtons.checkClick(*window);

    // Get mouse position
    sf::Vector2i mousePos = sf::Mouse::getPosition(*window);
    sf::Vector2u windowSize = window->getSize();

    // Screen scrolling based on mouse position
    const float scrollThreshold = 50.0f;
    if (mousePos.x < scrollThreshold) {
        scrollOffset -= 10.0f; // Scroll left
    }
    else if (mousePos.x > windowSize.x - scrollThreshold) {
        scrollOffset += 10.0f; // Scroll right
    }

    // Clamp the scroll offset to half the viewport width
    scrollOffset = std::clamp(scrollOffset, 0.0f, 360.0f);  // 360 = 720/2

    // Calculate new camera position using viewport dimensions instead of window size
    sf::Vector2f newCameraPos(scrollOffset + (VIEWPORT_WIDTH / 2), (VIEWPORT_HEIGHT / 2));
    Window::setCameraPosition(newCameraPos);
}

void Office::Render()
{
}

void Office::Destroy()
{
}
