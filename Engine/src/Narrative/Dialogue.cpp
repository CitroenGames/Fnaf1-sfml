#include "Narrative/Dialogue.h"

#include <utility>

bool DialogueRequirement::Allows(const MentalState &state) const {
    return state.Stability() >= minStability &&
           state.Stability() <= maxStability &&
           state.Ego() >= minEgo &&
           state.Ego() <= maxEgo &&
           state.Connection() >= minConnection &&
           state.Connection() <= maxConnection;
}

bool DialogueOption::IsAvailable(const MentalState &state) const {
    return requirement.Allows(state);
}

void DialogueGraph::Clear() {
    m_Nodes.clear();
    m_CurrentNodeId.clear();
}

void DialogueGraph::AddNode(DialogueNode node) {
    if (m_CurrentNodeId.empty()) {
        m_CurrentNodeId = node.id;
    }

    m_Nodes[node.id] = std::move(node);
}

bool DialogueGraph::SetCurrentNode(const std::string &nodeId) {
    if (m_Nodes.find(nodeId) == m_Nodes.end()) {
        return false;
    }

    m_CurrentNodeId = nodeId;
    return true;
}

const DialogueNode *DialogueGraph::GetNode(const std::string &nodeId) const {
    const auto it = m_Nodes.find(nodeId);
    if (it == m_Nodes.end()) {
        return nullptr;
    }

    return &it->second;
}

const DialogueNode *DialogueGraph::GetCurrentNode() const {
    return GetNode(m_CurrentNodeId);
}

std::vector<const DialogueOption *> DialogueGraph::GetAvailableOptions(const MentalState &state) const {
    std::vector<const DialogueOption *> options;
    const DialogueNode *node = GetCurrentNode();
    if (node == nullptr) {
        return options;
    }

    for (const DialogueOption &option: node->options) {
        if (option.IsAvailable(state)) {
            options.push_back(&option);
        }
    }

    return options;
}

std::optional<std::string> DialogueGraph::ChooseOption(const std::string &optionId, MentalState &state) {
    const DialogueNode *node = GetCurrentNode();
    if (node == nullptr) {
        return std::nullopt;
    }

    for (const DialogueOption &option: node->options) {
        if (option.id == optionId && option.IsAvailable(state)) {
            state.Apply(option.mentalDelta);
            if (!option.nextNodeId.empty()) {
                SetCurrentNode(option.nextNodeId);
            }
            return option.nextNodeId;
        }
    }

    return std::nullopt;
}

const std::string &DialogueGraph::CurrentNodeId() const {
    return m_CurrentNodeId;
}
