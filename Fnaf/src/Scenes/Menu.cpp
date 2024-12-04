#include "Menu.h"
#include "SFML/Window.hpp"
#include "Scene/SceneManager.h"
#include "Scenes/Gameplay.h"
#include "Assets/Resources.h"
#include "Graphics/LayerManager.h"
#include "Core/Window.h"
#include "LayerDefines.h"

bool ShouldShowNewsPaper = false;
bool ShouldSwitchScene = false;
int NewsPaperTimer = 0;
bool IsShowingNewsPaper = false;

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
    flipbook.AddFrame(Resources::GetTexture("Graphics/MenuMenu/FreddyBackground/MainMenu1.png"));
    flipbook.AddFrame(Resources::GetTexture("Graphics/MenuMenu/FreddyBackground/MainMenu2.png"));
    flipbook.AddFrame(Resources::GetTexture("Graphics/MenuMenu/FreddyBackground/MainMenu3.png"));
    flipbook.AddFrame(Resources::GetTexture("Graphics/MenuMenu/FreddyBackground/MainMenu4.png"));

    flipbook.SetPosition(-250, 0);
    // ------------------------------------------------------

    NewsPaperTexture = Resources::GetTexture("Graphics/MenuMenu/NewsPaper.png");

    m_Logo = Resources::GetTexture("Graphics/MenuMenu/Logo.png");

    m_LogoSprite = sf::Sprite(*m_Logo);
    m_LogoSprite.setPosition(100, 0);
    LayerManager::AddDrawable(BUTTON_LAYER, m_LogoSprite);
    flipbook.Play();

    newbutton.SetTexture("Graphics/MenuMenu/NewGame.png");
    newbutton.SetPosition(100, 300);
    newbutton.SetLayer(BUTTON_LAYER);
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

void Menu::ShowNewsPaper()
{
    NewsPaperTimer += 1;
    if (NewsPaperTimer > 1000)
    {
        ShouldShowNewsPaper = false;
        ShouldSwitchScene = true;
        return;
    }

    if(!IsShowingNewsPaper)
    {
        sf::Sprite sprite(*NewsPaperTexture);
        LayerManager::AddDrawable(4, sprite);
    }
    
}

void Menu::SwitchToGameplay()
{
    Destroy();
	SceneManager::QueueSwitchScene(std::make_shared<Gameplay>());
}

void Menu::FixedUpdate()
{
    if(ShouldShowNewsPaper)
    {
        ShowNewsPaper();
    }
    else if (ShouldSwitchScene)
    {
        SwitchToGameplay();
    }

    // TODO: window variable should not be needed...
    std::shared_ptr<sf::RenderWindow> window = Window::GetWindow();

	if (newbutton.IsClicked(*window))
	{
        ShouldShowNewsPaper = true;
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

    flipbook.Stop();
    flipbook.Cleanup();
}
