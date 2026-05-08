#include "Narrative/MentalState.h"

#include <algorithm>

namespace {
    int ClampStat(int value) {
        return std::clamp(value, 0, 100);
    }
}

MentalState::MentalState(int stability, int ego, int connection) {
    Set(stability, ego, connection);
}

void MentalState::Set(int stability, int ego, int connection) {
    m_Stability = ClampStat(stability);
    m_Ego = ClampStat(ego);
    m_Connection = ClampStat(connection);
}

void MentalState::Apply(const MentalStateDelta &delta) {
    Set(
        m_Stability + delta.stability,
        m_Ego + delta.ego,
        m_Connection + delta.connection);
}

int MentalState::Stability() const {
    return m_Stability;
}

int MentalState::Ego() const {
    return m_Ego;
}

int MentalState::Connection() const {
    return m_Connection;
}

nlohmann::json MentalState::ToJson() const {
    return {
        {"stability", m_Stability},
        {"ego", m_Ego},
        {"connection", m_Connection}
    };
}

MentalState MentalState::FromJson(const nlohmann::json &json) {
    MentalState state;
    state.Set(
        json.value("stability", state.Stability()),
        json.value("ego", state.Ego()),
        json.value("connection", state.Connection()));
    return state;
}
