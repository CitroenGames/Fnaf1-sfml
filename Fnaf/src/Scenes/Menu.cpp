#include "Menu.h"
#include "sfml/Window.hpp"
#include "Scene/SceneManager.h"
#include "scenes/Gameplay.h"
#include "assets/Resources.h"
#include "Graphics/LayerManager.h"
#include "Core/Window.h"

#define BUTTON_LAYER 2

Menu::Menu() : button(100, 300, "Assets/Graphics/MenuMenu/NewGame.png", BUTTON_LAYER)
{
}

void Menu::Init()
{
    bgaudio2 = Resources::GetMusic("Audio/Menu/darknessmusic.wav");
    if(bgaudio2)
	{
        bgaudio2->setLoop(true);
        bgaudio2->play();
        bgaudio2->setVolume(100.f);
	}

    texture1.loadFromFile("Assets/Graphics/MenuMenu/FreddyBackground/MainMenu1.png");
    texture2.loadFromFile("Assets/Graphics/MenuMenu/FreddyBackground/MainMenu2.png");
    texture3.loadFromFile("Assets/Graphics/MenuMenu/FreddyBackground/MainMenu3.png");
    texture4.loadFromFile("Assets/Graphics/MenuMenu/FreddyBackground/MainMenu4.png");


    flipbook = Flipbook(0, 0.2f, true);  // Passing true for looping
    flipbook.AddFrame(texture1);
    flipbook.AddFrame(texture2);
    flipbook.AddFrame(texture3);
    flipbook.AddFrame(texture4);

    flipbook.SetPosition(-250, 0);

    m_Logo.loadFromFile("Assets/Graphics/MenuMenu/Logo.png");
    m_LogoSprite = sf::Sprite(m_Logo);
    m_LogoSprite.setPosition(100, 0);
    LayerManager::AddDrawable(BUTTON_LAYER, m_LogoSprite);
    flipbook.Play();
}

void Menu::Update(double deltaTime)
{
    flipbook.Update(deltaTime);
    flipbook.RegisterToLayerManager();
}

void Menu::SwitchToGameplay()
{
    Destroy();
	SceneManager::SwitchScene(new Gameplay());
}

void Menu::FixedUpdate()
{
	sf::RenderWindow* window = Window::GetWindow();
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
	{
        SwitchToGameplay();
	}

    if (button.isClicked(*window)) {
        SwitchToGameplay();
    }

}

void Menu::Render()
{
}

void Menu::Destroy()
{
    LayerManager::Clear();
    if (bgaudio2)
    {
        bgaudio2->stop();
    }

    flipbook.Destroy();
    flipbook.Stop();
}
