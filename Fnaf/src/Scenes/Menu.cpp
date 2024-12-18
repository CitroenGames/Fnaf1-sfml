#include "Menu.h"
#include "SFML/Window.hpp"
#include "Scene/SceneManager.h"
#include "Scenes/Gameplay.h"
#include "Assets/Resources.h"
#include "Graphics/LayerManager.h"
#include "Core/Window.h"
#include "LayerDefines.h"
#include "Utils/Helpers.h"

int NewsPaperTimer = 0;
bool IsShowingNewsPaper = false;

// NOTE: this is in ticks not seconds so 66 ticks = 1 second
#if _DEBUG
#define SHOWNEWPAPERTIME 0.5 * 66 // 0.5 seconds
#else
#define SHOWNEWPAPERTIME 12 * 66 // 12 seconds
#endif

void Menu::Init()
{
    m_BgStatic = Resources::GetMusic("Audio/static2.wav");
    if (m_BgStatic)
    {
		m_BgStatic->play();
		m_BgStatic->setVolume(100.f);
    }
    m_MenuMusic = Resources::GetMusic("Audio/Menu/darknessmusic.wav");
    if(m_MenuMusic)
	{
        m_MenuMusic->setLoop(true);
        m_MenuMusic->play();
        m_MenuMusic->setVolume(100.f);
	}

    m_FreddyGlitchEffect = GlitchEffect(0); 
    for (int i = 1; i < 4; i++)
    {
		m_FreddyGlitchEffect.AddFrame(Resources::GetTexture("Graphics/MenuMenu/FreddyBackground/Frame" + std::to_string(i) + ".png"));
    }

    m_FreddyGlitchEffect.SetGlitchParameters(0.01f, 0.3f, 0.05f);

    // Apply transparency to all static noise frames
    for (int i = 1; i <= 8; i++)
    {
        m_NoiseTextures.push_back(MakeTextureTransparent(Resources::GetTexture("Graphics/Static/Noise" + std::to_string(i) + ".png"), 0.15f));
    }
    
    m_StaticGlitchEffect = GlitchEffect(1);
    for(int i = 0; i < m_NoiseTextures.size(); i++)
	{
		m_StaticGlitchEffect.AddFrame(m_NoiseTextures[i]);
	}

    m_StaticGlitchEffect.SetGlitchParameters(1.f, 1.5f, 0.01f);

    NewsPaperTexture = Resources::GetTexture("Graphics/MenuMenu/NewsPaper.png");
    NewsPaperSprite = sf::Sprite(*NewsPaperTexture);

    m_Logo = ProcessText(Resources::GetTexture("Graphics/MenuMenu/Logo.png"));

    m_LogoSprite = sf::Sprite(*m_Logo);
    m_LogoSprite.setPosition(100, 100);
    LayerManager::AddDrawable(BUTTON_LAYER, &m_LogoSprite);

    newbutton.SetTexture(ProcessText(Resources::GetTexture("Graphics/MenuMenu/NewGame.png")));
    newbutton.SetPosition(100, 400);
    newbutton.SetLayer(BUTTON_LAYER);
}

void Menu::Update(double deltaTime)
{
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
    // TODO: if we add custom nights we need to tell the scene what ai levels we want...
	SceneManager::QueueSwitchScene(std::make_shared<Gameplay>());
}

void Menu::FixedUpdate()
{
    if (IsShowingNewsPaper)
    {
        NewsPaperTimer += 1;
        if (NewsPaperTimer >= SHOWNEWPAPERTIME) // tick rate is 66 so this takes about 1.5 seconds
        {
            SwitchToGameplay();
        }
        return;
    }

    m_FreddyGlitchEffect.Update();
    m_StaticGlitchEffect.Update();

    // TODO: window variable should not be needed...
    std::shared_ptr<sf::RenderWindow> window = Window::GetWindow();

	if (newbutton.IsClicked(*window))
	{
        LayerManager::AddDrawable(4, &NewsPaperSprite);
        IsShowingNewsPaper = true;
	}
}

void Menu::Render()
{
}

void Menu::Destroy()
{
    LayerManager::Clear();
    if (m_MenuMusic)
    {
        m_MenuMusic->stop();
        m_MenuMusic.reset();
    }
}
