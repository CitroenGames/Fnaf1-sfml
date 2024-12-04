#include "Menu.h"
#include "SFML/Window.hpp"
#include "Scene/SceneManager.h"
#include "Scenes/Gameplay.h"
#include "Assets/Resources.h"
#include "Graphics/LayerManager.h"
#include "Core/Window.h"
#include "LayerDefines.h"


void Menu::Init()
{
    bgaudio2 = Resources::GetMusic("Audio/Menu/darknessmusic.wav");
    if(bgaudio2)
	{
        bgaudio2->setLoop(true);
        bgaudio2->play();
        bgaudio2->setVolume(100.f);
	}

    // NOTE: THIS IS NOT HOW IT LOOKS LIKE IN THE ACTUAL GAME
    flipbook = Flipbook(0, 0.2f, true);  // Passing true for looping
    flipbook.AddFrame(*Resources::GetTexture("Graphics/MenuMenu/FreddyBackground/MainMenu1.png"));
    flipbook.AddFrame(*Resources::GetTexture("Graphics/MenuMenu/FreddyBackground/MainMenu2.png"));
    flipbook.AddFrame(*Resources::GetTexture("Graphics/MenuMenu/FreddyBackground/MainMenu3.png"));
    flipbook.AddFrame(*Resources::GetTexture("Graphics/MenuMenu/FreddyBackground/MainMenu4.png"));

    flipbook.SetPosition(-250, 0);
    // ------------------------------------------------------

    m_Logo = Resources::GetTexture("Graphics/MenuMenu/Logo.png");

    m_LogoSprite = sf::Sprite(*m_Logo);
    m_LogoSprite.setPosition(100, 0);
    LayerManager::AddDrawable(BUTTON_LAYER, m_LogoSprite);
    flipbook.Play();

    button.SetTexture("Graphics/MenuMenu/NewGame.png");
    button.SetPosition(100, 300);
    button.SetLayer(BUTTON_LAYER);
}

void Menu::Update(double deltaTime)
{
    flipbook.Update(deltaTime);
    flipbook.RegisterToLayerManager();

    //accumulatedTime += (float)deltaTime;

    //switch (m_State) {
    //case FADE_IN:
    //    if (fadeIn.update(deltaTime)) {
    //        m_State = WAIT;
    //    }
    //    break;
    //case WAIT:
    //    accumulatedTime += deltaTime;
    //    if (accumulatedTime >= waitTime) {
    //        m_State = FADE_OUT;
    //        accumulatedTime = sf::Time::Zero;
    //    }
    //    break;
    //case FADE_OUT:
    //    if (fadeOut.update(deltaTime)) {
    //        m_State = DONE;
    //    }
    //    break;
    //case DONE:
    //    // Fade out completed, you can do other things here or exit the application.
    //    break;
    //}
}

void Menu::SwitchToGameplay()
{
    Destroy();
	SceneManager::QueueSwitchScene(std::make_shared<Gameplay>());
}

void Menu::FixedUpdate()
{
    std::shared_ptr<sf::RenderWindow> window = Window::GetWindow();
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
	{
        SwitchToGameplay();
	}

    if (button.IsClicked(*window)) {
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
