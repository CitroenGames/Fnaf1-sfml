#include "Scene/SceneManager.h"
#include "Core/Application.h"
#include "Scenes/Gameplay.h"
#include "Scenes/Menu.h"
#include "pak.h"
#include "filesystem"
#include "Assets/Resources.h"

int main() {
    Pakker p("example_key");

    // Create PAK if it doesn't exist
    if (!std::filesystem::exists("assets.pak")) {
        std::cout << "Packing assets.pak" << std::endl;
        // Check if assets folder exists
        if (!std::filesystem::exists("assets")) {
            std::cerr << "Error: assets folder not found" << std::endl;
            return 1;
        }
        if (!p.CreatePakFromFolder("assets.pak", "assets")) {
            std::cerr << "Error: Failed to create assets.pak" << std::endl;
            return 1;
        }
    }

    // List contents of the PAK
    if (!p.ListPak("assets.pak")) {
        std::cerr << "Error: Failed to list contents of assets.pak" << std::endl;
        return 1;
    }

    // Set Pakker instance in Resources
    Resources::SetPakker(&p);

    // Load Resources
    Resources::Load("assets.pak");
    
    Application::Init();
    SceneManager::QueueSwitchScene(new Menu());
    Application::Run();
    Application::Destroy();
    return 0;
}
