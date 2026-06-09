#pragma once
#include <array>

class Keyboard {
public:
    void SetKeyboardDown(int index);
    void SetKeyboardUp(int index);
    bool IsKeyDown(int index);
    bool IsKeyJustReleased(int index);
    int GetFirstKeyJustReleased();
    void UpdateKeys();
private:
    std::array<bool, 16> keyboardPressed{};
    std::array<bool, 16> keyboardPressedLastFrame{};
    std::array<bool, 16> keyboardReleasedThisFrame{};
};
