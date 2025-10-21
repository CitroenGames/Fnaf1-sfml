#include "Menu.h"
#include "Scene/SceneManager.h"
#include "Scenes/Gameplay.h"
#include "Assets/Resources.h"
#include "Graphics/LayerManager.h"
#include "LayerDefines.h"
#include "Utils/Helpers.h"
#include "Audio/AudioManager.h"

int NewsPaperTimer = 0;

// NOTE: this is in ticks not seconds so 66 ticks = 1 second
#if _DEBUG
#define SHOWNEWPAPERTIME 0.5 * 66 // 0.5 seconds
#else
#define SHOWNEWPAPERTIME 12 * 66 // 12 seconds
#endif

Menu::Menu() {
    // Preload audio - no longer keeping music objects directly in the Menu class
    Resources::GetMusic(STATIC_AUDIO_KEY);
    Resources::GetMusic(MENU_MUSIC_KEY);

    // Noise textures
    for (int i = 1; i <= 8; i++) {
        m_NoiseTextures.push_back(MakeTextureTransparent(
            Resources::GetTexture("Graphics/Static/Noise" + std::to_string(i) + ".png"),
            0.15f
        ));
    }

    // Load & Setup white textures
    for (int i = 1; i <= 8; i++) {
        m_WhiteTextures.push_back(RemoveBlackBackground(
            Resources::GetTexture("Graphics/White/WhiteThing" + std::to_string(i) + ".png")
        ));
    }

    // Load & Setup Freddy glitch effect
    m_FreddyGlitchEffect = GlitchEffect(0);
    for (int i = 1; i < 4; i++) {
        m_FreddyGlitchEffect.AddFrame(Resources::GetTexture(
            "Graphics/MainMenu/FreddyBackground/Frame" + std::to_string(i) + ".png"
        ));
    }

    // Load textures
    m_Logo = ProcessText(Resources::GetTexture("Graphics/MainMenu/Logo.png"));
    NewsPaperTexture = Resources::GetTexture("Graphics/MainMenu/NewsPaper.png");
    m_WarningMessageTexture = ProcessText(Resources::GetTexture("Graphics/MainMenu/WarningMessage.png"));

    // Setup warning message sprite
    m_WarningMessageSprite.setTexture(*m_WarningMessageTexture);
    m_WarningMessageSprite.setPosition(sf::Vector2f(
        (Window::GetWindow()->getSize().x - m_WarningMessageSprite.getGlobalBounds().size.x) / 2,
        (Window::GetWindow()->getSize().y - m_WarningMessageSprite.getGlobalBounds().size.y) / 2
    ));

    // load loading screen sprite
    m_LoadingScreenTexture = Resources::GetTexture("Graphics/Loading.png");

    // load time and night text
    font = Resources::GetFont("Font/five-nights-at-freddys.ttf");

    // Prepare logo sprite
    m_LogoSprite = sf::Sprite(*m_Logo);
    m_LogoSprite.setPosition(sf::Vector2f(100, 100));
}

void Menu::Init() {
    ShowMainMenuElements();

    // Start audio using AudioManager
    AudioManager::GetInstance().PlayMusic(STATIC_AUDIO_KEY, false, 100.0f);
    AudioManager::GetInstance().PlayMusic(MENU_MUSIC_KEY, true, 100.0f);

    m_TimeText.setFont(*font);
    m_TimeText.setString("12:00 AM");
    m_TimeText.setCharacterSize(75);
    m_TimeText.setFillColor(sf::Color::White);
    m_TimeText.setPosition(sf::Vector2f(
        (Window::GetWindow()->getSize().x - m_TimeText.getGlobalBounds().size.x) / 2,
        Window::GetWindow()->getSize().y / 2 - 25
    ));

    m_NightText.setFont(*font);
    m_NightText.setString("1st Night");
    m_NightText.setCharacterSize(75);
    m_NightText.setFillColor(sf::Color::White);
    m_NightText.setPosition(sf::Vector2f(
        (Window::GetWindow()->getSize().x - m_NightText.getGlobalBounds().size.x) / 2,
        Window::GetWindow()->getSize().y / 2 + 25
    ));

    // Setup loading screen sprite
    m_LoadingScreenSprite.setTexture(*m_LoadingScreenTexture);
    m_LoadingScreenSprite.setPosition(sf::Vector2f(
        Window::GetWindow()->getSize().x - m_LoadingScreenSprite.getGlobalBounds().size.x,
        Window::GetWindow()->getSize().y - m_LoadingScreenSprite.getGlobalBounds().size.y
    ));

    // Setup white glitch effect
    m_WhiteGlitchEffect = GlitchEffect(2);
    for (int i = 0; i < m_WhiteTextures.size(); i++) {
        m_WhiteGlitchEffect.AddFrame(m_WhiteTextures[i]);
    }

    // Setup static glitch effect
    m_StaticGlitchEffect = GlitchEffect(1);
    for (int i = 0; i < m_NoiseTextures.size(); i++) {
        m_StaticGlitchEffect.AddFrame(m_NoiseTextures[i]);
    }

    // Configure glitch parameters
    m_FreddyGlitchEffect.SetGlitchParameters(0.01f, 0.3f, 0.05f);
    m_StaticGlitchEffect.SetGlitchParameters(1.f, 1.5f, 0.01f);
    m_WhiteGlitchEffect.SetGlitchParameters(1.f, 1.5f, 0.15f);

    // Prepare newspaper sprite
    NewsPaperSprite = sf::Sprite(*NewsPaperTexture);
    NewsPaperSprite.setPosition(sf::Vector2f(
        (Window::GetWindow()->getSize().x - NewsPaperSprite.getGlobalBounds().size.y) / 2,
        (Window::GetWindow()->getSize().y - NewsPaperSprite.getGlobalBounds().size.y) / 2
    ));

    // Setup new game button
    newbutton.SetTexture(ProcessText(Resources::GetTexture("Graphics/MainMenu/NewGame.png")));
    newbutton.SetPosition(100, 400);
}

void Menu::HideGlitchEffects() {
    // Set glitch effects to an invisible layer
    m_FreddyGlitchEffect.SetLayer(-1);
    m_FreddyGlitchEffect.Kill();
    m_StaticGlitchEffect.SetLayer(-1);
    m_StaticGlitchEffect.Kill();
    m_WhiteGlitchEffect.SetLayer(-1);
    m_WhiteGlitchEffect.Kill();
}

void Menu::ShowGlitchEffects() {
    // Restore glitch effects to their original layers
    m_FreddyGlitchEffect.SetLayer(0);
    m_StaticGlitchEffect.SetLayer(1);
    m_WhiteGlitchEffect.SetLayer(2);
}

void Menu::SwitchState(GameplayTransitionState state) {
    m_GameplayTransitionState = state;
    NewsPaperTimer = 0; // Reset timer
}

void Menu::HideAllMenuElements() {
    // Remove all menu elements from the layer manager
    LayerManager::RemoveDrawable(&m_LogoSprite);
    LayerManager::RemoveDrawable(&NewsPaperSprite);
    LayerManager::RemoveDrawable(&m_TimeText);
    LayerManager::RemoveDrawable(&m_NightText);
    LayerManager::RemoveDrawable(&m_LoadingScreenSprite);
    LayerManager::RemoveDrawable(&m_WarningMessageSprite);

    // Remove buttons from layer
    newbutton.SetVisible(false);

    // Hide glitch effects
    HideGlitchEffects();
}

void Menu::ShowMainMenuElements() {
    // Add main menu elements to the layer manager
    LayerManager::AddDrawable(MENU_BUTTON_LAYER, &m_LogoSprite);

    // Add new game button to the visible layer
    newbutton.SetLayer(MENU_BUTTON_LAYER);

    // Show glitch effects
    ShowGlitchEffects();
}

void Menu::Update(double deltaTime) {
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

void Menu::SwitchToGameplay() {
    // Clean up all transition elements before switching scenes
    HideAllMenuElements();

    // Switch to gameplay scene
    SceneManager::QueueSwitchScene(std::make_shared<Gameplay>());
}

void Menu::FixedUpdate() {
    // TODO: idk remove this stupid seconds thing
    NewsPaperTimer += 1;
    const float seconds = NewsPaperTimer / 66.0f; // Convert ticks to seconds
    switch (m_GameplayTransitionState) {
        case MAIN_MENU: {
            m_FreddyGlitchEffect.Update();
            m_StaticGlitchEffect.Update();
            m_WhiteGlitchEffect.Update();

            auto window = Window::GetWindow();

            if (newbutton.IsClicked(*window)) {
                // Hide all menu elements when transitioning to newspaper
                HideAllMenuElements();

                // Show only the newspaper
                LayerManager::AddDrawable(4, &NewsPaperSprite);
                SwitchState(NEWSPAPER);
            }
            break;
        }
        case FADE_IN: {
            break;
        }
        case NEWSPAPER: {
            if (seconds >= NEWSPAPER_DURATION) {
                // Switch to time display
                LayerManager::RemoveDrawable(&NewsPaperSprite);
                LayerManager::AddDrawable(MENU_BUTTON_LAYER, &m_TimeText);
                LayerManager::AddDrawable(MENU_BUTTON_LAYER, &m_NightText);
                SwitchState(TIME_DISPLAY);
            }
            break;
        }

        case FADE_OUT: {
            break;
        }

        case TIME_DISPLAY: {
            if (seconds >= TIME_DISPLAY_DURATION) {
                // Switch to loading screen
                LayerManager::RemoveDrawable(&m_TimeText);
                LayerManager::RemoveDrawable(&m_NightText);
                LayerManager::AddDrawable(MENU_BUTTON_LAYER, &m_LoadingScreenSprite);
                SwitchState(LOADING_SCREEN);
            }
            break;
        }

        case LOADING_SCREEN: {
            if (seconds >= LOADING_SCREEN_DURATION) {
                // Transition complete, switch to gameplay
                SwitchState(COMPLETE);
                SwitchToGameplay();
            }
            break;
        }


        default:
            break;
    }
}
