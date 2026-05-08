#include "Rhythm/RhythmTrack.h"

#include <algorithm>
#include <cmath>
#include <limits>

void RhythmTrack::Clear() {
    m_Notes.clear();
    ResetPerformance();
}

void RhythmTrack::AddNote(RhythmNote note) {
    m_Notes.push_back({note, false});
}

void RhythmTrack::SortNotes() {
    std::sort(m_Notes.begin(), m_Notes.end(), [](const NoteState &lhs, const NoteState &rhs) {
        if (lhs.note.time == rhs.note.time) {
            return lhs.note.lane < rhs.note.lane;
        }

        return lhs.note.time < rhs.note.time;
    });
}

void RhythmTrack::ResetPerformance() {
    for (NoteState &state: m_Notes) {
        state.judged = false;
    }

    m_Score = 0;
    m_Combo = 0;
    m_MaxCombo = 0;
    m_HitCount = 0;
    m_MissCount = 0;
}

void RhythmTrack::SetHitWindow(RhythmHitWindow window) {
    m_Window = window;
}

void RhythmTrack::SetAssistModeEnabled(bool enabled) {
    m_AssistModeEnabled = enabled;
}

bool RhythmTrack::IsAssistModeEnabled() const {
    return m_AssistModeEnabled;
}

RhythmHitResult RhythmTrack::Hit(int lane, double playbackTime) {
    RhythmHitResult result;
    RhythmHitWindow window = ActiveWindow();

    NoteState *bestState = nullptr;
    double bestError = std::numeric_limits<double>::max();
    for (NoteState &state: m_Notes) {
        if (state.judged || state.note.lane != lane) {
            continue;
        }

        const double error = std::abs(playbackTime - state.note.time);
        if (error <= window.ok && error < bestError) {
            bestState = &state;
            bestError = error;
        }
    }

    if (bestState == nullptr) {
        m_Combo = 0;
        ++m_MissCount;
        return result;
    }

    bestState->judged = true;
    result.error = playbackTime - bestState->note.time;
    if (bestError <= window.perfect) {
        result.grade = RhythmHitGrade::Perfect;
    } else if (bestError <= window.good) {
        result.grade = RhythmHitGrade::Good;
    } else {
        result.grade = RhythmHitGrade::Ok;
    }

    result.scoreDelta = ScoreForGrade(result.grade);
    m_Score += result.scoreDelta;
    ++m_Combo;
    ++m_HitCount;
    m_MaxCombo = std::max(m_MaxCombo, m_Combo);
    result.combo = m_Combo;
    return result;
}

void RhythmTrack::MarkMisses(double playbackTime) {
    const RhythmHitWindow window = ActiveWindow();
    for (NoteState &state: m_Notes) {
        if (!state.judged && playbackTime - state.note.time > window.ok) {
            state.judged = true;
            m_Combo = 0;
            ++m_MissCount;
        }
    }
}

int RhythmTrack::Score() const {
    return m_Score;
}

int RhythmTrack::Combo() const {
    return m_Combo;
}

int RhythmTrack::MaxCombo() const {
    return m_MaxCombo;
}

int RhythmTrack::HitCount() const {
    return m_HitCount;
}

int RhythmTrack::MissCount() const {
    return m_MissCount;
}

double RhythmTrack::Accuracy() const {
    const int total = m_HitCount + m_MissCount;
    if (total == 0) {
        return 1.0;
    }

    return static_cast<double>(m_HitCount) / static_cast<double>(total);
}

std::vector<RhythmNote> RhythmTrack::Notes() const {
    std::vector<RhythmNote> notes;
    notes.reserve(m_Notes.size());
    for (const NoteState &state: m_Notes) {
        notes.push_back(state.note);
    }
    return notes;
}

RhythmHitWindow RhythmTrack::ActiveWindow() const {
    if (!m_AssistModeEnabled) {
        return m_Window;
    }

    return {
        m_Window.perfect * 1.75,
        m_Window.good * 1.75,
        m_Window.ok * 1.75
    };
}

int RhythmTrack::ScoreForGrade(RhythmHitGrade grade) {
    switch (grade) {
        case RhythmHitGrade::Perfect:
            return 1000;
        case RhythmHitGrade::Good:
            return 700;
        case RhythmHitGrade::Ok:
            return 400;
        case RhythmHitGrade::Miss:
        default:
            return 0;
    }
}
