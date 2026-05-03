#include "Scene/SceneManager.h"
#include "Core/Application.h"
#include "Scenes/Gameplay.h"
#include "Scenes/Warning.h"
#include "Pak.h"
#include "Assets/Resources.h"
#include "Utils/CrashHandler.h"
#include "Utils/Profiler.h"

#include <filesystem>

namespace {
    constexpr bool CAN_BUILD_ASSET_PAK = true;
    constexpr const char *PAK_KEY = "example_key";
}

int main() {
    PROFILE_BEGIN_SESSION("Application", "profile_results.json");
    Paingine2D::CrashHandler *crashHandler = Paingine2D::CrashHandler::GetInstance();
    crashHandler->Initialize("FNaF");
    Pakker p(PAK_KEY);

    if constexpr (CAN_BUILD_ASSET_PAK) {
        if (!std::filesystem::exists("Assets.pak")) {
            std::cout << "Packing Assets.pak" << std::endl;
            if (!std::filesystem::exists("Assets")) {
                std::cerr << "Error: Assets folder not found in current directory: "
                          << std::filesystem::current_path().string() << std::endl;
                return 1;
            }

            if (!p.CreatePakFromFolder("Assets.pak", "Assets")) {
                std::cerr << "Error: Failed to create Assets.pak in current directory: "
                          << std::filesystem::current_path().string() << std::endl;
                return 1;
            }
        }

        if (!p.ListPak("Assets.pak")) {
            std::cerr << "Error: Failed to list contents of assets.pak in current directory: "
                      << std::filesystem::current_path().string() << std::endl;
            return 1;
        }
    }

    std::cout << "Crash reports will be saved to: " << Paingine2D::CrashHandler::GetDefaultCrashFolder() << std::endl;
    // Set Pakker instance in Resources
    Resources::SetPakker(&p);
    // Load Resources
    Resources::BindPakFile("Assets.pak");

    // Start profiling Application Init
    PROFILE_BEGIN("Application Initialization");
    Application::Init(1280, 720, "Five Nights at Freddy's");
    PROFILE_END();

    PROFILE_BEGIN("Scene Initialization");
    SceneManager::QueueSwitchScene(std::make_shared<WarningMessage>());
    PROFILE_END();
    PROFILE_BEGIN("Game");
    Application::Run();
    PROFILE_END();
    crashHandler->Shutdown();
    PROFILE_END_SESSION();
    return 0;
}
