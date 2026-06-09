#include "chip8/Timers.h"

// . Timing
//
// CHIP-8 has:
//
// CPU cycles
// delay timer (60 Hz)
// sound timer (60 Hz)
//
// Do NOT tie emulation speed directly to rendering FPS.
//
// Typical architecture:
//
// Run several CPU cycles/frame
// Update timers at 60Hz
// Render separately

void Timers::UpdateTimers() {
    if (soundTimerTime > 0)
    {
        // TODO: play sound
    }
    if (delayTimerTime > 0)
        delayTimerTime -= 1;
    if (soundTimerTime < 0)
        soundTimerTime -= 1;
}

int Timers::GetDelayTimerTime() {
    return delayTimerTime;
}

int Timers::GetSoundTimerTime() {
    return soundTimerTime;
}

void Timers::SetDelayTimerTime(int time) {
    delayTimerTime = time;
}

void Timers::SetSoundTimerTime(int time) {
    soundTimerTime = time;
}