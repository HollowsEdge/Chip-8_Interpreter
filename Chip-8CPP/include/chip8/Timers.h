#pragma once

class Timers {
    public:
        void UpdateTimers();
        int GetDelayTimerTime();
        int GetSoundTimerTime();
        void SetDelayTimerTime(int time);
        void SetSoundTimerTime(int time);
    private:
        int delayTimerTime = 0;
        int soundTimerTime = 0;
};