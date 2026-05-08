#include "Accessibility/SubtitleTrack.h"

#include <algorithm>
#include <utility>

void SubtitleTrack::SetEnabled(bool enabled) {
    m_Enabled = enabled;
}

bool SubtitleTrack::IsEnabled() const {
    return m_Enabled;
}

void SubtitleTrack::SetAmbientEnabled(bool enabled) {
    m_AmbientEnabled = enabled;
}

bool SubtitleTrack::IsAmbientEnabled() const {
    return m_AmbientEnabled;
}

void SubtitleTrack::Clear() {
    m_Cues.clear();
}

void SubtitleTrack::AddCue(SubtitleCue cue) {
    m_Cues.push_back(std::move(cue));
    std::sort(m_Cues.begin(), m_Cues.end(), [](const SubtitleCue &lhs, const SubtitleCue &rhs) {
        return lhs.startTime < rhs.startTime;
    });
}

std::vector<SubtitleCue> SubtitleTrack::GetActiveCues(double playbackTime) const {
    std::vector<SubtitleCue> active;
    if (!m_Enabled) {
        return active;
    }

    for (const SubtitleCue &cue: m_Cues) {
        if (cue.startTime > playbackTime) {
            break;
        }

        if (cue.endTime >= playbackTime && (!cue.ambient || m_AmbientEnabled)) {
            active.push_back(cue);
        }
    }

    return active;
}

std::string SubtitleTrack::GetActiveText(double playbackTime, const std::string &separator) const {
    std::string text;
    for (const SubtitleCue &cue: GetActiveCues(playbackTime)) {
        if (!text.empty()) {
            text += separator;
        }

        if (!cue.speaker.empty()) {
            text += cue.speaker;
            text += ": ";
        }
        text += cue.text;
    }

    return text;
}
