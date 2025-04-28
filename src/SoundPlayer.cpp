#include "../include/SoundPlayer.h"
#include <MeMCore.h>

// Ensure MelodyType is properly scoped
using MelodyType = SoundPlayer::MelodyType;

static constexpr int NOTE_SILENCE = 0; // Silence

static constexpr int NOTE_C3 = 130;
static constexpr int NOTE_D3 = 147;
static constexpr int NOTE_E3 = 165;
static constexpr int NOTE_F3 = 175;
static constexpr int NOTE_G3 = 196;
static constexpr int NOTE_A3 = 220;
static constexpr int NOTE_B3 = 247;

static constexpr int NOTE_C4 = 262;
static constexpr int NOTE_D4 = 294;
static constexpr int NOTE_E4 = 330;
static constexpr int NOTE_F4 = 349;
static constexpr int NOTE_G4 = 392;
static constexpr int NOTE_A4 = 440;
static constexpr int NOTE_B4 = 494;

static constexpr int NOTE_C5 = 523;
static constexpr int NOTE_D5 = 587;
static constexpr int NOTE_E5 = 659;
static constexpr int NOTE_F5 = 698;
static constexpr int NOTE_G5 = 784;
static constexpr int NOTE_A5 = 880;
static constexpr int NOTE_B5 = 988;

// Define the static array
static constexpr int notes[] = {
    NOTE_C3, NOTE_D3, NOTE_E3, NOTE_F3, NOTE_G3, NOTE_A3, NOTE_B3,
    NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_A4, NOTE_B4,
    NOTE_C5, NOTE_D5, NOTE_E5, NOTE_F5, NOTE_G5, NOTE_A5, NOTE_B5
};

void SoundPlayer::playNoteAtIndex(int noteIndex) {
    buzzer.tone(notes[noteIndex], 100); // Play the note at the given index
}

int SoundPlayer::getNotesCount() {
    return sizeof(notes) / sizeof(notes[0]); // Return the number of notes
}

struct Melody {
    MelodyType type;
    const int* notes;
    const int* durations;
    int length;
};

static constexpr int obstacleNotes[] = {NOTE_F4, NOTE_D4};
static constexpr int obstacleDurations[] = {100, 100};

static constexpr int explorationNotes[] = {NOTE_SILENCE, NOTE_G4, NOTE_A4, NOTE_B4, NOTE_C5, NOTE_B4, NOTE_A4, NOTE_G4};
static constexpr int explorationDurations[] = {1000, 200, 200, 200, 300, 200, 200, 300};

static constexpr int raceStartingNotes[] = {NOTE_C4, NOTE_C4, NOTE_C4, NOTE_C5};
static constexpr int raceStartingDurations[] = {300, 300, 300, 600};

static constexpr int raceCompletedNotes[] = {NOTE_C4, NOTE_E4, NOTE_G4, NOTE_C5};
static constexpr int raceCompletedDurations[] = {300, 300, 300, 600};

static constexpr Melody melodies[] = {
    {SoundPlayer::OBSTACLE_ENCOUNTERED, obstacleNotes, obstacleDurations, 3},
    {SoundPlayer::EXPLORATION_COMPLETED, explorationNotes, explorationDurations, 8},
    {SoundPlayer::RACE_STARTING, raceStartingNotes, raceStartingDurations, 4},
    {SoundPlayer::RACE_COMPLETED, raceCompletedNotes, raceCompletedDurations, 3}
};

void SoundPlayer::playMelody(MelodyType type) {    
    for (const Melody& melody : melodies) {
        if (melody.type == type) {
            for (int i = 0; i < melody.length; ++i) {
                buzzer.tone(melody.notes[i], melody.durations[i]);
                delay(melody.durations[i]);
            }
            delay(300); // Add a short delay after the melody

            break;
        }
    }
}