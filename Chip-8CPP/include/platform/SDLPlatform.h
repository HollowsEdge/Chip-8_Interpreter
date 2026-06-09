#pragma once

#include "chip8/Chip8.h"
#include "chip8/Display.h"
#include <string>
#include <cmath>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

class SDLPlatform {
    public:
        SDLPlatform(const std::string& windowName, int windowWidth, int windowHeight);
        bool HandleInput(Keyboard& keyboard);
        void Render(Display& disp);
        void Shutdown();
        constexpr static int MINIMUM_WINDOW_SIZE_WIDTH{ 640 };
        constexpr static int MINIMUM_WINDOW_SIZE_HEIGHT{ 360 };
        constexpr static SDL_WindowFlags WINDOW_FLAGS = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
    private:
        SDL_Window* window{ nullptr };
        SDL_Renderer* renderer{ nullptr };
};