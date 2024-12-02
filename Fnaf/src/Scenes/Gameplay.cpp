#include "Gameplay.h"
#include "sfml/Window.hpp"
#include <SFML/Graphics.hpp>
#include "Core/Window.h"
#include "Graphics/LayerManager.h"
#include "assets/Resources.h"
#include "Power.h"
#include "Imgui.h"

void Gameplay::Init()
{
    auto entity = CreateEntity();
    entity->AddComponent<PowerIndicator>()->Init();
    entity->AddComponent<Office>()->Init();
    auto officeComponent = entity->GetComponent<Office>();
    //AddComponent(officeComponent);
    
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
    Scene::FixedUpdate();
}

void Gameplay::Update(double deltaTime)
{
    Scene::Update(deltaTime);
}

void Gameplay::Render()
{
    ImGui::Begin("Gameplay");
	ImGui::Text("Gameplay");
	ImGui::End();
}

void Gameplay::Destroy()
{
    m_Office.Destroy();
    Scene::Destroy();
}
