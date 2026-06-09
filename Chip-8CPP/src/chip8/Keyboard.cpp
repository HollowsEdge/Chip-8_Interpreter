#include "chip8/Keyboard.h"
#include <algorithm>

void Keyboard::SetKeyboardDown(int index) {
    keyboardPressed[index] = true;
}

void Keyboard::SetKeyboardUp(int index) {
    keyboardPressed[index] = false;
}

bool Keyboard::IsKeyDown(int index) {
    return keyboardPressed[index];
}

bool Keyboard::IsKeyJustReleased(int index) {
    return keyboardReleasedThisFrame[index];
}

int Keyboard::GetFirstKeyJustReleased() {
    for (int index = 0; index < keyboardReleasedThisFrame.size(); index++) {
        if (keyboardReleasedThisFrame[index] == true) return index;
    }
    return -1;
}

void Keyboard::UpdateKeys() {
    for (int index = 0; index < keyboardPressed.size(); index++) {
        keyboardReleasedThisFrame[index] = keyboardPressedLastFrame[index] && !keyboardPressed[index];
        keyboardPressedLastFrame[index] = keyboardPressed[index];
    }
}