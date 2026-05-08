#pragma once

#include <filesystem>
#include <optional>
#include <string>

#include "Save/SaveGame.h"

class SaveSystem {
public:
    static void SetSaveDirectory(std::filesystem::path directory);
    static const std::filesystem::path &GetSaveDirectory();

    static bool SaveSlot(const SaveGame &save);
    static std::optional<SaveGame> LoadSlot(const std::string &slot);
    static bool DeleteSlot(const std::string &slot);
    static bool SlotExists(const std::string &slot);

private:
    static std::filesystem::path SlotPath(const std::string &slot);
};
