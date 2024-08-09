#include "Gameplay.h"
#include "sfml/Window.hpp"
#include <SFML/Graphics.hpp>
#include "Core/Window.h"
#include "Graphics/LayerManager.h"
#include "assets/Resources.h"

void Gameplay::Init()
{
    officeTexture = Resources::GetTexture("Graphics/Office/NormalOffice.png");
    buttonTexture = Resources::GetTexture("Graphics/Office/button.png");
    doorTexture = Resources::GetTexture("Graphics/Office/door.png");

    // Load music
    bgaudio1 = Resources::GetMusic("Audio/Ambience/ambience2.wav");
    bgaudio1->setLoop(true);
    bgaudio1->play();
    bgaudio1->setVolume(100.f);

    bgaudio2 = Resources::GetMusic("Audio/Ambience/EerieAmbienceLargeSca_MV005.wav");
    bgaudio2->setLoop(true);
    bgaudio2->play();
    bgaudio2->setVolume(50.f);

    m_FanBuzzing = Resources::GetMusic("Audio/Office/Buzz_Fan_Florescent2.wav");
	m_FanBuzzing->setLoop(true);
	m_FanBuzzing->play();
	m_FanBuzzing->setVolume(40.f);

    // Create sprites
    officeSprite = sf::Sprite(*officeTexture);
    leftButtonSprite = sf::Sprite(*buttonTexture);
    rightButtonSprite = sf::Sprite(*buttonTexture);
    leftDoorSprite = sf::Sprite(*doorTexture);
    rightDoorSprite = sf::Sprite(*doorTexture);

    LayerManager::AddDrawable(0, officeSprite);
    LayerManager::AddDrawable(1, leftButtonSprite);
    LayerManager::AddDrawable(1, rightButtonSprite);
    LayerManager::AddDrawable(1, leftDoorSprite);
    LayerManager::AddDrawable(1, rightDoorSprite);

    // Scale the office sprite
    officeSprite.setScale(0.9f, 0.9f);

    // Initial positions
    leftButtonSprite.setPosition(50, 500);   // Example position
    rightButtonSprite.setPosition(700, 500); // Example position
    leftDoorSprite.setPosition(100, 500);    // Example position
    rightDoorSprite.setPosition(650, 500);   // Example position
}

void Gameplay::FixedUpdate()
{
    // Handle events
    sf::Event event;
    sf::RenderWindow* window = Window::GetWindow();

    while (window->pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            Window::Destroy();
        }
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

    officeSprite.setPosition(scrollOffset, 0);
    officeSprite.setRotation(lookAngle);

    leftButtonSprite.setPosition(50 + scrollOffset, 500);
    leftButtonSprite.setRotation(lookAngle);

    rightButtonSprite.setPosition(700 + scrollOffset, 500);
    rightButtonSprite.setRotation(lookAngle);

    leftDoorSprite.setPosition(100 + scrollOffset, 500);
    leftDoorSprite.setRotation(lookAngle);

    rightDoorSprite.setPosition(650 + scrollOffset, 500);
    rightDoorSprite.setRotation(lookAngle);
}

void Gameplay::Update(double deltaTime)
{
}

void Gameplay::Render()
{

}

void Gameplay::Destroy()
{
    Scene::Destroy();
}
