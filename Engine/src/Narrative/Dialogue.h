#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "Narrative/MentalState.h"

enum class DialogueTone {
    Honest,
    Strategic,
    Withdrawn,
    Vulnerable
};

struct DialogueRequirement {
    int minStability = 0;
    int maxStability = 100;
    int minEgo = 0;
    int maxEgo = 100;
    int minConnection = 0;
    int maxConnection = 100;

    bool Allows(const MentalState &state) const;
};

struct DialogueOption {
    std::string id;
    std::string text;
    DialogueTone tone = DialogueTone::Strategic;
    MentalStateDelta mentalDelta;
    DialogueRequirement requirement;
    std::string nextNodeId;

    bool IsAvailable(const MentalState &state) const;
};

struct DialogueNode {
    std::string id;
    std::string speaker;
    std::string line;
    std::vector<DialogueOption> options;
};

class DialogueGraph {
public:
    void Clear();
    void AddNode(DialogueNode node);
    bool SetCurrentNode(const std::string &nodeId);

    const DialogueNode *GetNode(const std::string &nodeId) const;
    const DialogueNode *GetCurrentNode() const;
    std::vector<const DialogueOption *> GetAvailableOptions(const MentalState &state) const;

    std::optional<std::string> ChooseOption(const std::string &optionId, MentalState &state);
    const std::string &CurrentNodeId() const;

private:
    std::unordered_map<std::string, DialogueNode> m_Nodes;
    std::string m_CurrentNodeId;
};
