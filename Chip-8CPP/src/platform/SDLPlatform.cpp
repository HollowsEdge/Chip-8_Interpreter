#include "platform/SDLPlatform.h"

SDLPlatform::SDLPlatform(const std::string &windowName, int windowWidth, int windowHeight)
{
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return;
    }

    if (!SDL_CreateWindowAndRenderer(
            windowName.c_str(),
            windowWidth,
            windowHeight,
            WINDOW_FLAGS,
            &window,
            &renderer)) {

        SDL_Log("Window creation failed: %s", SDL_GetError());
        return;
            }

    SDL_SetWindowMinimumSize(
        window,
        MINIMUM_WINDOW_SIZE_WIDTH,
        MINIMUM_WINDOW_SIZE_HEIGHT
    );

    SDL_SetRenderVSync(renderer, 1);
}

bool SDLPlatform::HandleInput(Keyboard& keyboard) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            return false;
        }
        if (event.type == SDL_EVENT_KEY_DOWN) {
            switch (event.key.scancode) {
                case SDL_SCANCODE_1:
                    keyboard.SetKeyboardDown(0x1);
                    break;
                case SDL_SCANCODE_2:
                    keyboard.SetKeyboardDown(0x2);
                    break;
                case SDL_SCANCODE_3:
                    keyboard.SetKeyboardDown(0x3);
                    break;
                case SDL_SCANCODE_4:
                    keyboard.SetKeyboardDown(0xC);
                    break;
                case SDL_SCANCODE_Q:
                    keyboard.SetKeyboardDown(0x4);
                    break;
                case SDL_SCANCODE_W:
                    keyboard.SetKeyboardDown(0x5);
                    break;
                case SDL_SCANCODE_E:
                    keyboard.SetKeyboardDown(0x6);
                    break;
                case SDL_SCANCODE_R:
                    keyboard.SetKeyboardDown(0xD);
                    break;
                case SDL_SCANCODE_A:
                    keyboard.SetKeyboardDown(0x7);
                    break;
                case SDL_SCANCODE_S:
                    keyboard.SetKeyboardDown(0x8);
                    break;
                case SDL_SCANCODE_D:
                    keyboard.SetKeyboardDown(0x9);
                    break;
                case SDL_SCANCODE_F:
                    keyboard.SetKeyboardDown(0xE);
                    break;
                case SDL_SCANCODE_Z:
                    keyboard.SetKeyboardDown(0xA);
                    break;
                case SDL_SCANCODE_X:
                    keyboard.SetKeyboardDown(0x0);
                    break;
                case SDL_SCANCODE_C:
                    keyboard.SetKeyboardDown(0xB);
                    break;
                case SDL_SCANCODE_V:
                    keyboard.SetKeyboardDown(0xF);
                    break;
                default:
                    break;
            }
        }
        if (event.type == SDL_EVENT_KEY_UP) {
            switch (event.key.scancode) {
                case SDL_SCANCODE_1:
                    keyboard.SetKeyboardUp(0x1);
                    break;
                case SDL_SCANCODE_2:
                    keyboard.SetKeyboardUp(0x2);
                    break;
                case SDL_SCANCODE_3:
                    keyboard.SetKeyboardUp(0x3);
                    break;
                case SDL_SCANCODE_4:
                    keyboard.SetKeyboardUp(0xC);
                    break;
                case SDL_SCANCODE_Q:
                    keyboard.SetKeyboardUp(0x4);
                    break;
                case SDL_SCANCODE_W:
                    keyboard.SetKeyboardUp(0x5);
                    break;
                case SDL_SCANCODE_E:
                    keyboard.SetKeyboardUp(0x6);
                    break;
                case SDL_SCANCODE_R:
                    keyboard.SetKeyboardUp(0xD);
                    break;
                case SDL_SCANCODE_A:
                    keyboard.SetKeyboardUp(0x7);
                    break;
                case SDL_SCANCODE_S:
                    keyboard.SetKeyboardUp(0x8);
                    break;
                case SDL_SCANCODE_D:
                    keyboard.SetKeyboardUp(0x9);
                    break;
                case SDL_SCANCODE_F:
                    keyboard.SetKeyboardUp(0xE);
                    break;
                case SDL_SCANCODE_Z:
                    keyboard.SetKeyboardUp(0xA);
                    break;
                case SDL_SCANCODE_X:
                    keyboard.SetKeyboardUp(0x0);
                    break;
                case SDL_SCANCODE_C:
                    keyboard.SetKeyboardUp(0xB);
                    break;
                case SDL_SCANCODE_V:
                    keyboard.SetKeyboardUp(0xF);
                    break;
                default:
                    break;
            }
        }
    }
    return true;
}

void SDLPlatform::Render(Display& display) {
    // Takes CHIP-8 framebuffer
    // Draws pixels using SDL
    std::array<uint8_t, 4> pixelColor{0};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);


    uint8_t* displayBuffer1 = display.GetDisplayBuffer(1);
    uint8_t* displayBuffer2 = display.GetDisplayBuffer(2);

    int windowHeight{};
    int windowWidth{};
    SDL_GetWindowSizeInPixels(window, &windowWidth, &windowHeight);

    float scale = floorf(std::min(
        static_cast<float>(windowWidth) / display.GetScreenWidth(),
        static_cast<float>(windowHeight) / display.GetScreenHeight()
    ));
    float offsetX = (windowWidth - (display.GetScreenWidth() * scale)) * 0.5f;
    float offsetY = (windowHeight - (display.GetScreenHeight() * scale)) * 0.5f;

    SDL_FRect rect{};
    rect.w = scale;
    rect.h = scale;

    for (size_t i = 0; i < display.GetScreenWidth(); i++) {
        for (size_t j = 0; j < display.GetScreenHeight(); j++) {
            pixelColor = display.GetPixelColor(i, j);
            SDL_SetRenderDrawColor(renderer, pixelColor[0], pixelColor[1], pixelColor[2], pixelColor[3]);
            rect.x = offsetX + i * scale;
            rect.y = offsetY + j * scale;
            SDL_RenderFillRect(renderer, &rect);
        }
    }
    SDL_RenderPresent(renderer);
}

void SDLPlatform::Shutdown() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}