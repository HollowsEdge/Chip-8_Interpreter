#include "chip8/Chip8.h"
#include "platform/SDLPlatform.h"
#include <chrono>
#include <iostream>

constexpr int INIT_WINDOW_WIDTH{ 1280 };
constexpr int INIT_WINDOW_HEIGHT{ 720 };
constexpr double CPU_HZ{10000};
constexpr double TIMER_HZ{60.0};

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cerr << "Invalid argument amount. " << std::endl;
        return 1;
    }

    Chip8 chip8;
    SDLPlatform platform("CHIP-8 Emulator", INIT_WINDOW_WIDTH, INIT_WINDOW_HEIGHT);

    chip8.LoadROM(argv[1]);

    bool running = true;
    auto lastTime{std::chrono::high_resolution_clock::now()};
    double deltaTime{ 0 };
    auto currentTime{std::chrono::high_resolution_clock::now()};

    double cpuAccumulator = 0.0;
    double timerAccumulator = 0.0;

    while (running && !chip8.GetExitProgram())
    {
        currentTime = std::chrono::high_resolution_clock::now();
        deltaTime = std::chrono::duration<double>(currentTime - lastTime).count();
        lastTime = currentTime;

        cpuAccumulator += deltaTime;
        timerAccumulator += deltaTime;

        chip8.keyboard.UpdateKeys();
        running = platform.HandleInput(chip8.keyboard);

        while (cpuAccumulator >= 1.0 / CPU_HZ)
        {
            chip8.Cycle();
            if (chip8.GetExitProgram())
                break;
            cpuAccumulator -= 1.0 / CPU_HZ;
        }

        while (timerAccumulator >= 1.0 / TIMER_HZ)
        {
            chip8.UpdateTimers();
            timerAccumulator -= 1.0 / TIMER_HZ;
        }

        platform.Render(chip8.GetDisplay());
    }

    platform.Shutdown();
    return 0;
}