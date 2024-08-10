#include "office.h"
#include "Core/Window.h"
#include "Graphics/LayerManager.h"
#include "assets/Resources.h"

void LeftTopButtonCallback()
{
	std::cout << "Left Top Button Pressed" << std::endl;
}

void LeftBottomButtonCallback()
{
	std::cout << "Right Bottom Button Pressed" << std::endl;
}

void Office::Init()
{
    m_OfficeTexture = Resources::GetTexture("Graphics/Office/NormalOffice.png");
    m_LeftButtonTexture = Resources::GetTexture("Graphics/Office/button.png");
    m_DoorTexture = Resources::GetTexture("Graphics/Office/door.png");

    // Create sprites
    m_OfficeSprite = sf::Sprite(*m_OfficeTexture);
    m_LeftButtonSprite = sf::Sprite(*m_LeftButtonTexture);
    m_RightButtonSprite = sf::Sprite(*m_LeftButtonTexture);
    m_LeftDoorSprite = sf::Sprite(*m_DoorTexture);
    m_RightDoorSprite = sf::Sprite(*m_DoorTexture);

    m_LeftButtons.SetTexture(m_LeftButtonTexture);
    m_LeftButtons.SetLayer(2);
    m_LeftButtons.SetCallbacks(LeftTopButtonCallback, LeftBottomButtonCallback);

    LayerManager::AddDrawable(0, m_OfficeSprite);
    LayerManager::AddDrawable(1, m_LeftDoorSprite);
    LayerManager::AddDrawable(1, m_RightDoorSprite);

    // Scale the office sprite
    m_OfficeSprite.setScale(0.9f, 0.9f);

    // Initial positions
    m_RightButtonSprite.setPosition(700, 500); // Example position
    m_LeftDoorSprite.setPosition(100, 500);    // Example position
    m_RightDoorSprite.setPosition(650, 500);   // Example position
}

void Office::Update(double deltaTime)
{
}

void Office::FixedUpdate()
{
    // Handle events
    sf::Event event;
    sf::RenderWindow* window = Window::GetWindow();

    while (window->pollEvent(event)) {
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Right) {
                scrollOffset -= 10.0f; // Scroll right
            }
            if (event.key.code == sf::Keyboard::Left) {
                scrollOffset += 10.0f; // Scroll left
            }
            // Handle button press
            if (event.key.code == sf::Keyboard::Space) {
                // Example button press logic
            }
        }
    }

    m_LeftButtons.checkClick(*window);

    // FNAF-style mouse look and screen scrolling
    sf::Vector2i mousePos = sf::Mouse::getPosition(*window);
    sf::Vector2u windowSize = window->getSize();

    // Calculate the mouse position relative to the center of the window
    float mouseX = static_cast<float>(mousePos.x - windowSize.x / 2);
    float mouseY = static_cast<float>(mousePos.y - windowSize.y / 2);

    // Scroll the screen based on the mouse position
    const float scrollThreshold = 50.0f; // Adjust the threshold as needed
    if (mousePos.x < scrollThreshold) {
        scrollOffset += 10.0f; // Scroll left
    }
    else if (mousePos.x > windowSize.x - scrollThreshold) {
        scrollOffset -= 10.0f; // Scroll right
    }

    //// limit the scroll offset to the size of the office texture
    //if (scrollOffset < 0.0f) {
    //    scrollOffset = 0.0f;
    //}
    //else if (scrollOffset > (m_OfficeTexture->getSize().x * 2)) {
    //    scrollOffset = m_OfficeTexture->getSize().x * 2;
    //}

    m_OfficeSprite.setPosition(scrollOffset, 0);
    m_LeftButtons.SetPosition(scrollOffset, 250);
    m_RightButtonSprite.setPosition(700 + scrollOffset, 500);
    m_LeftDoorSprite.setPosition(100 + scrollOffset, 500);
    m_RightDoorSprite.setPosition(650 + scrollOffset, 500);
}

void Office::Render()
{
}

void Office::Destroy()
{
}
