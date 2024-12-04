#include "Scene/SceneManager.h"
#include "Core/Application.h"
#include "Scenes/Gameplay.h"
#include "Scenes/Menu.h"
#include "Pak.h"
#include "filesystem"
#include "Assets/Resources.h"

#define CAN_BUILD_ASSET_PAK 1

int main() {
    Pakker p("example_key");

#if CAN_BUILD_ASSET_PAK
    // Create PAK if it doesn't exist
    if (!std::filesystem::exists("Assets.pak")) {
        std::cout << "Packing Assets.pak" << std::endl;
        // Check if assets folder exists
        if (!std::filesystem::exists("Assets")) {
            std::cerr << "Error: Assets folder not found" << std::endl;
            return 1;
        }
        if (!p.CreatePakFromFolder("Assets.pak", "Assets")) {
            std::cerr << "Error: Failed to create Assets.pak" << std::endl;
            return 1;
        }
    }

    // List contents of the PAK
    if (!p.ListPak("Assets.pak")) {
        std::cerr << "Error: Failed to list contents of assets.pak" << std::endl;
        return 1;
    }
#endif

    // Set Pakker instance in Resources
    Resources::SetPakker(&p);

    // Load Resources
    Resources::Load("Assets.pak");
    
    Application::Init();
    SceneManager::QueueSwitchScene(std::make_shared<Menu>());
    Application::Run();
    Application::Destroy();
    return 0;
}
