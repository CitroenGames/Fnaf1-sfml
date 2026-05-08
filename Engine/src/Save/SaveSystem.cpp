#include "Save/SaveSystem.h"

#include <fstream>
#include <utility>

namespace {
    std::filesystem::path g_SaveDirectory = "Saves";

    std::string SafeSlotName(const std::string &slot) {
        std::string safe;
        safe.reserve(slot.size());
        for (char c: slot) {
            if ((c >= 'A' && c <= 'Z') ||
                (c >= 'a' && c <= 'z') ||
                (c >= '0' && c <= '9') ||
                c == '-' ||
                c == '_') {
                safe.push_back(c);
            } else {
                safe.push_back('_');
            }
        }

        return safe.empty() ? "autosave" : safe;
    }
}

void SaveSystem::SetSaveDirectory(std::filesystem::path directory) {
    g_SaveDirectory = std::move(directory);
}

const std::filesystem::path &SaveSystem::GetSaveDirectory() {
    return g_SaveDirectory;
}

bool SaveSystem::SaveSlot(const SaveGame &save) {
    std::error_code error;
    std::filesystem::create_directories(g_SaveDirectory, error);
    if (error) {
        return false;
    }

    std::ofstream file(SlotPath(save.slot), std::ios::binary | std::ios::trunc);
    if (!file) {
        return false;
    }

    file << save.ToJson().dump(4);
    return file.good();
}

std::optional<SaveGame> SaveSystem::LoadSlot(const std::string &slot) {
    std::ifstream file(SlotPath(slot), std::ios::binary);
    if (!file) {
        return std::nullopt;
    }

    try {
        nlohmann::json json;
        file >> json;
        return SaveGame::FromJson(json);
    } catch (...) {
        return std::nullopt;
    }
}

bool SaveSystem::DeleteSlot(const std::string &slot) {
    std::error_code error;
    return std::filesystem::remove(SlotPath(slot), error) && !error;
}

bool SaveSystem::SlotExists(const std::string &slot) {
    return std::filesystem::exists(SlotPath(slot));
}

std::filesystem::path SaveSystem::SlotPath(const std::string &slot) {
    return g_SaveDirectory / (SafeSlotName(slot) + ".json");
}
