#pragma once

#include <string>
#include <vector>

struct SubtitleCue {
    double startTime = 0.0;
    double endTime = 0.0;
    std::string speaker;
    std::string text;
    bool ambient = false;
};

class SubtitleTrack {
public:
    void SetEnabled(bool enabled);
    bool IsEnabled() const;

    void SetAmbientEnabled(bool enabled);
    bool IsAmbientEnabled() const;

    void Clear();
    void AddCue(SubtitleCue cue);
    std::vector<SubtitleCue> GetActiveCues(double playbackTime) const;
    std::string GetActiveText(double playbackTime, const std::string &separator = "\n") const;

private:
    std::vector<SubtitleCue> m_Cues;
    bool m_Enabled = true;
    bool m_AmbientEnabled = true;
};
