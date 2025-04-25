#ifndef SOUND_PLAYER_H
#define SOUND_PLAYER_H

#include <MeMCore.h>
#include <MeBuzzer.h>
#include <stdint.h>
#include "GlobalConstants.h"

class SoundPlayer {
public:
    // Enum for different melody types    
    enum MelodyType {
        OBSTACLE_ENCOUNTERED = 0,
        EXPLORATION_COMPLETED = 1,
        RACE_STARTING = 2,
        RACE_COMPLETED = 3
    };
private:
    MeBuzzer& buzzer; // Reference to the buzzer object
public:

    SoundPlayer(MeBuzzer& buzzer) : buzzer(buzzer) {}

    //declare the methods
    void playNoteAtIndex(int noteIndex);
    static int getNotesCount();
    void playMelody(MelodyType type);
    
};



#endif