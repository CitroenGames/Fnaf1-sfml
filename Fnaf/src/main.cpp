#include "Scene/SceneManager.h"
#include "Core/Application.h"
#include "Scenes/Gameplay.h"
#include "Scenes/Warning.h"
#include "Pak.h"
#include "Assets/Resources.h"
#include "Utils/CrashHandler.h"
#include "Utils/Profiler.h"

#include <filesystem>
#include <iostream>
#include <system_error>
#include <vector>

#if defined(__APPLE__)
#include <mach-o/dyld.h>
#elif defined(__linux__)
#include <unistd.h>
#endif

namespace {
    constexpr bool CAN_BUILD_ASSET_PAK = true;
    constexpr const char *PAK_KEY = "example_key";

    std::filesystem::path GetExecutablePath(const char *argv0) {
#if defined(__APPLE__)
        uint32_t size = 0;
        _NSGetExecutablePath(nullptr, &size);

        std::vector<char> buffer(size);
        if (_NSGetExecutablePath(buffer.data(), &size) == 0) {
            return std::filesystem::weakly_canonical(buffer.data());
        }
#elif defined(__linux__)
        std::vector<char> buffer(4096);
        const ssize_t length = readlink("/proc/self/exe", buffer.data(), buffer.size() - 1);
        if (length > 0) {
            buffer[static_cast<size_t>(length)] = '\0';
            return std::filesystem::weakly_canonical(buffer.data());
        }
#endif

        return std::filesystem::weakly_canonical(argv0);
    }

    void SetWorkingDirectoryToExecutable(const char *argv0) {
        const auto executablePath = GetExecutablePath(argv0);
        if (executablePath.empty()) {
            return;
        }

        std::error_code error;
        std::filesystem::current_path(executablePath.parent_path(), error);
        if (error) {
            std::cerr << "Warning: Failed to set working directory to executable folder: "
                      << error.message() << std::endl;
        }
    }

    bool ShouldBuildAssetPak(const std::filesystem::path &pakFile, const std::filesystem::path &assetsFolder) {
        if (!std::filesystem::exists(pakFile)) {
            return true;
        }

        if (!std::filesystem::exists(assetsFolder)) {
            return false;
        }

        const auto pakWriteTime = std::filesystem::last_write_time(pakFile);
        for (const auto &entry : std::filesystem::recursive_directory_iterator(assetsFolder)) {
            if (entry.is_regular_file() && entry.last_write_time() > pakWriteTime) {
                return true;
            }
        }

        return false;
    }

    std::filesystem::path FindAssetsFolder() {
        const std::filesystem::path runtimeAssets = "Assets";
        if (std::filesystem::exists(runtimeAssets)) {
            return runtimeAssets;
        }

        const std::filesystem::path sourceTreeAssets = "Fnaf/Assets";
        if (std::filesystem::exists(sourceTreeAssets)) {
            return sourceTreeAssets;
        }

        return runtimeAssets;
    }
}

int main(int argc, char *argv[]) {
    SetWorkingDirectoryToExecutable(argc > 0 ? argv[0] : "");

    PROFILE_BEGIN_SESSION("Application", "profile_results.json");
    Paingine2D::CrashHandler *crashHandler = Paingine2D::CrashHandler::GetInstance();
    crashHandler->Initialize("FNaF");
    Pakker p(PAK_KEY);

    if constexpr (CAN_BUILD_ASSET_PAK) {
        const std::filesystem::path pakFile = "Assets.pak";
        const std::filesystem::path assetsFolder = FindAssetsFolder();

        if (ShouldBuildAssetPak(pakFile, assetsFolder)) {
            std::cout << "Packing Assets.pak" << std::endl;
            if (!std::filesystem::exists(assetsFolder)) {
                std::cerr << "Error: Assets folder not found in current directory: "
                          << std::filesystem::current_path().string() << std::endl;
                return 1;
            }

            const PakOptions pakOptions{};
            if (!p.CreatePakFromFolder(pakFile.string(), assetsFolder.string(), pakOptions)) {
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
