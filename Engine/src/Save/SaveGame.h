#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>

#include <nlohmann/json.hpp>

#include "Narrative/MentalState.h"

struct SaveGame {
    std::string slot = "autosave";
    std::string chapterId;
    std::string sceneId;
    std::string checkpointId;
    MentalState mentalState;
    std::unordered_set<std::string> flags;
    std::unordered_map<std::string, double> numbers;
    std::unordered_map<std::string, std::string> strings;

    nlohmann::json ToJson() const;
    static SaveGame FromJson(const nlohmann::json &json);
};
