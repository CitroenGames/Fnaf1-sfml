#pragma once

#include <nlohmann/json.hpp>

struct MentalStateDelta {
    int stability = 0;
    int ego = 0;
    int connection = 0;
};

class MentalState {
public:
    MentalState() = default;
    MentalState(int stability, int ego, int connection);

    void Set(int stability, int ego, int connection);
    void Apply(const MentalStateDelta &delta);

    int Stability() const;
    int Ego() const;
    int Connection() const;

    nlohmann::json ToJson() const;
    static MentalState FromJson(const nlohmann::json &json);

private:
    int m_Stability = 50;
    int m_Ego = 50;
    int m_Connection = 50;
};
