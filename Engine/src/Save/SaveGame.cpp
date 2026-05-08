#include "Save/SaveGame.h"

#include <algorithm>
#include <vector>

nlohmann::json SaveGame::ToJson() const {
    std::vector<std::string> sortedFlags(flags.begin(), flags.end());
    std::sort(sortedFlags.begin(), sortedFlags.end());

    return {
        {"slot", slot},
        {"chapter_id", chapterId},
        {"scene_id", sceneId},
        {"checkpoint_id", checkpointId},
        {"mental_state", mentalState.ToJson()},
        {"flags", sortedFlags},
        {"numbers", numbers},
        {"strings", strings}
    };
}

SaveGame SaveGame::FromJson(const nlohmann::json &json) {
    SaveGame save;
    save.slot = json.value("slot", save.slot);
    save.chapterId = json.value("chapter_id", std::string());
    save.sceneId = json.value("scene_id", std::string());
    save.checkpointId = json.value("checkpoint_id", std::string());

    if (json.contains("mental_state")) {
        save.mentalState = MentalState::FromJson(json.at("mental_state"));
    }

    if (json.contains("flags") && json.at("flags").is_array()) {
        for (const auto &flag: json.at("flags")) {
            save.flags.insert(flag.get<std::string>());
        }
    }

    if (json.contains("numbers") && json.at("numbers").is_object()) {
        save.numbers = json.at("numbers").get<std::unordered_map<std::string, double> >();
    }

    if (json.contains("strings") && json.at("strings").is_object()) {
        save.strings = json.at("strings").get<std::unordered_map<std::string, std::string> >();
    }

    return save;
}
