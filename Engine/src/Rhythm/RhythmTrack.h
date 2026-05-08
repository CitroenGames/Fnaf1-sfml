#pragma once

#include <cstddef>
#include <vector>

enum class RhythmNoteType {
    Tap,
    HoldStart,
    HoldEnd
};

enum class RhythmHitGrade {
    Miss,
    Ok,
    Good,
    Perfect
};

struct RhythmNote {
    double time = 0.0;
    int lane = 0;
    RhythmNoteType type = RhythmNoteType::Tap;
};

struct RhythmHitWindow {
    double perfect = 0.04;
    double good = 0.08;
    double ok = 0.12;
};

struct RhythmHitResult {
    RhythmHitGrade grade = RhythmHitGrade::Miss;
    double error = 0.0;
    int scoreDelta = 0;
    int combo = 0;
};

class RhythmTrack {
public:
    void Clear();
    void AddNote(RhythmNote note);
    void SortNotes();
    void ResetPerformance();

    void SetHitWindow(RhythmHitWindow window);
    void SetAssistModeEnabled(bool enabled);
    bool IsAssistModeEnabled() const;

    RhythmHitResult Hit(int lane, double playbackTime);
    void MarkMisses(double playbackTime);

    int Score() const;
    int Combo() const;
    int MaxCombo() const;
    int HitCount() const;
    int MissCount() const;
    double Accuracy() const;

    std::vector<RhythmNote> Notes() const;

private:
    struct NoteState {
        RhythmNote note;
        bool judged = false;
    };

    RhythmHitWindow ActiveWindow() const;
    static int ScoreForGrade(RhythmHitGrade grade);

    std::vector<NoteState> m_Notes;
    RhythmHitWindow m_Window;
    bool m_AssistModeEnabled = false;
    int m_Score = 0;
    int m_Combo = 0;
    int m_MaxCombo = 0;
    int m_HitCount = 0;
    int m_MissCount = 0;
};
