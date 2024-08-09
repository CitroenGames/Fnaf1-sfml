#include "Gameplay.h"
#include "sfml/Window.hpp"
#include <SFML/Graphics.hpp>
#include "Core/Window.h"
#include "Graphics/LayerManager.h"
#include "assets/Resources.h"

void Gameplay::Init()
{
    auto entity = CreateEntity();
    entity->AddComponent<Office>()->Init();;
    auto officeComponent = entity->GetComponent<Office>();
    world.addTickableComponent(officeComponent);
    
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
}

void Gameplay::FixedUpdate()
{
    world.fixedupdate();
}

void Gameplay::Update(double deltaTime)
{
    world.update(deltaTime);
}

void Gameplay::Render()
{
}

void Gameplay::Destroy()
{
    m_Office.Destroy();
    Scene::Destroy();
}
