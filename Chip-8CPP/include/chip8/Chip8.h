#pragma once

#include <string>

#include "chip8/CPU.h"
#include "chip8/Memory.h"
#include "chip8/Display.h"
#include "chip8/Keyboard.h"
#include "chip8/Timers.h"

class Chip8 {
    public:
        Chip8();
        void LoadROM(const std::string& path);
        void Cycle();
        void UpdateTimers();
        bool GetExitProgram();

        Display& GetDisplay();
        Keyboard keyboard;

        enum Platform {
            Chip_8,
            Super_Chip_8,
            XO_Chip
        };

        constexpr static Platform selectedPlatform = XO_Chip;

    private:
        Memory memory;
        Display display;
        Timers timers;
        CPU cpu;
};