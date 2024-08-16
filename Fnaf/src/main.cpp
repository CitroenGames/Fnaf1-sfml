#include "Scene/SceneManager.h"
#include "Core/Application.h"
#include "Scenes/Gameplay.h"
#include "Scenes/Menu.h"
#include "pak.h"
#include "filesystem"
#include "Assets/Resources.h"

int main() {
    // check if we have assets.pak if not pack it
    Pakker p;
    if (!std::filesystem::exists("assets.pak")) {
		std::cout << "Packing assets.pak" << std::endl;
        // check if assets folder exists
        if(!std::filesystem::exists("assets")) {
            std::cerr << "Error: assets folder not found" << std::endl;
			return 1;
		}
        p.CreatePakFromFolder("assets.pak", "assets");
    }

    p.ListPak("assets.pak");

    Resources::Load("assets.pak");
    
    Application::Init();
    SceneManager::QueueSwitchScene(new Menu());
    Application::Run();
    Application::Destroy();
    return 0;
}
