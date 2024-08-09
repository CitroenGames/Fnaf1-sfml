#include "Menu.h"
#include "sfml/Window.hpp"
#include "Scene/SceneManager.h"
#include "scenes/Gameplay.h"
#include "assets/Resources.h"
#include "layers/LayerManager.h"

void Menu::Init()
{
    bgaudio2 = Resources::GetMusic("Audio/Menu/darknessmusic.wav");
    if(bgaudio2)
	{
        bgaudio2->setLoop(true);
        bgaudio2->play();
        bgaudio2->setVolume(100.f);
	}

    texture1.loadFromFile("Assets/Graphics/UI/MainMenu1.png");
    texture2.loadFromFile("Assets/Graphics/UI/MainMenu2.png");
    texture3.loadFromFile("Assets/Graphics/UI/MainMenu3.png");
    texture4.loadFromFile("Assets/Graphics/UI/MainMenu4.png");

    flipbook = Flipbook(1, 0.2f, true);  // Passing true for looping
    flipbook.addFrame(texture1);
    flipbook.addFrame(texture2);
    flipbook.addFrame(texture3);
    flipbook.addFrame(texture4);

    flipbook.setPosition(-450, 0);

    flipbook.play();
}

void Menu::Update(double deltaTime)
{
    flipbook.update(deltaTime);
    flipbook.registerToLayerManager();
}

void Menu::FixedUpdate()
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
	{
		SceneManager::SwitchScene(new Gameplay());
	}
}

void Menu::Render()
{
}

void Menu::Destroy()
{
    if (bgaudio2)
    {
        bgaudio2->stop();
    }

    flipbook.Destroy();
    flipbook.stop();
    Scene::Destroy();
}
