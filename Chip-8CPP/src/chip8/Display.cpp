#include "chip8/Display.h"
#include "chip8/Chip8.h"
#include <cstring>

uint8_t* Display::GetDisplayBuffer(uint8_t buffer) {
    return buffer == 1 ? displayBuffer1.data() : displayBuffer2.data();
}

bool Display::DrawSprite(int posX, int posY, int spriteWidth, int spriteHeight, uint8_t* spriteData) {
    bool overlap = false;
    // Wrap start position
    posX %= currWidth;
    posY %= currHeight;

    int bytesPerRow = (spriteWidth == 16) ? 2 : 1;
    int planeOffset = spriteHeight * bytesPerRow;

    for (size_t j = 0; j < spriteHeight; j++) {
        uint16_t spriteRow1 = 0;
        uint16_t spriteRow2 = 0;

        if (bitPlane & 1) {
            if (spriteWidth == 16) {
                spriteRow1 = (spriteData[j * 2] << 8) | spriteData[j * 2 + 1];
            } else {
                spriteRow1 = spriteData[j];
            }
        }

        if (bitPlane & 2) {

            int offset = (bitPlane == 3) ? planeOffset : 0;

            if (spriteWidth == 16) {
                spriteRow2 = (spriteData[offset + j * 2] << 8) | spriteData[offset + j * 2 + 1];
            } else {
                spriteRow2 = spriteData[offset + j];
            }
        }

        for (size_t i = 0; i < spriteWidth; i++) {
            int targetPosX = posX + i;
            int targetPosY = posY + j;

            // Check OOB
            if (targetPosX >= currWidth || targetPosY >= currHeight) {
                if (Chip8::selectedPlatform == Chip8::Platform::XO_Chip) {
                    targetPosX %= currWidth;
                    targetPosY %= currHeight;
                }else {
                    continue;
                }
            }

            if (bitPlane & 1) {
                uint8_t spritePixel1 = (spriteRow1 >> (spriteWidth - 1 - i)) & 1;
                if (displayBuffer1[(targetPosY) * currWidth + (targetPosX)] == 1 && spritePixel1)
                    overlap = true;
                displayBuffer1[(targetPosY) * currWidth + (targetPosX)] ^= spritePixel1;
            }

            if (bitPlane & 2) {
                uint8_t spritePixel2 = (spriteRow2 >> (spriteWidth - 1 - i)) & 1;
                if (displayBuffer2[(targetPosY) * currWidth + (targetPosX)] == 1 && spritePixel2)
                    overlap = true;
                displayBuffer2[(targetPosY) * currWidth + (targetPosX)] ^= spritePixel2;
            }
        }
    }
    return overlap;
}

void Display::Clear() {
    if (bitPlane == 1 || bitPlane == 3)
        displayBuffer1.fill(0);
    if (bitPlane == 2 || bitPlane == 3)
        displayBuffer2.fill(0);
}

void Display::ScrollScreenUp(uint8_t pixelAmount) {
    if (bitPlane == 1 || bitPlane == 3) {
        std::memmove(
                displayBuffer1.data(),
                displayBuffer1.data() + pixelAmount * currWidth,
                (currHeight - pixelAmount) * currWidth
            );

        // Clear bottom rows
        std::fill(
            displayBuffer1.begin() + (currHeight - pixelAmount) * currWidth,
            displayBuffer1.end(),
            0
        );
    }
    if (bitPlane == 2 || bitPlane == 3) {
        std::memmove(
                displayBuffer2.data(),
                displayBuffer2.data() + pixelAmount * currWidth,
                (currHeight - pixelAmount) * currWidth
            );

        // Clear bottom rows
        std::fill(
            displayBuffer2.begin() + (currHeight - pixelAmount) * currWidth,
            displayBuffer2.end(),
            0
        );
    }
}

void Display::ScrollScreenDown(uint8_t pixelAmount) {
    if (bitPlane == 1 || bitPlane == 3) {
        std::memmove(
            displayBuffer1.data() + pixelAmount * currWidth,
            displayBuffer1.data(),
            (currHeight - pixelAmount) * currWidth
        );

        std::fill(
            displayBuffer1.begin(),
            displayBuffer1.begin() + pixelAmount * currWidth,
            0
        );
    }
    if (bitPlane == 2 || bitPlane == 3) {
        std::memmove(
            displayBuffer2.data() + pixelAmount * currWidth,
            displayBuffer2.data(),
            (currHeight - pixelAmount) * currWidth
        );

        std::fill(
            displayBuffer2.begin(),
            displayBuffer2.begin() + pixelAmount * currWidth,
            0
        );
    }
}

void Display::ScrollScreenLeft() {
    for (int y = 0; y < currHeight; ++y)
    {
        if (bitPlane == 1 || bitPlane == 3) {
            uint8_t* row = displayBuffer1.data() + y * currWidth;

            // Move pixels left
            std::memmove(
                row,
                row + 4,
                currWidth - 4
            );

            // Clear right side
            std::memset(
                row + (currWidth - 4),
                0,
                4
            );
        }
        if (bitPlane == 2 || bitPlane == 3) {
            uint8_t* row = displayBuffer2.data() + y * currWidth;

            // Move pixels left
            std::memmove(
                row,
                row + 4,
                currWidth - 4
            );

            // Clear right side
            std::memset(
                row + (currWidth - 4),
                0,
                4
            );
        }
    }
}

void Display::ScrollScreenRight() {
    for (int y = 0; y < currHeight; ++y)
    {
        if (bitPlane == 1 || bitPlane == 3) {
            uint8_t* row = displayBuffer1.data() + y * currWidth;

            // Move pixels right
            std::memmove(
                row + 4,
                row,
                currWidth - 4
            );

            // Clear left side
            std::memset(row, 0, 4);
        }
        if (bitPlane == 2 || bitPlane == 3) {
            uint8_t* row = displayBuffer2.data() + y * currWidth;

            // Move pixels right
            std::memmove(
                row + 4,
                row,
                currWidth - 4
            );

            // Clear left side
            std::memset(row, 0, 4);
        }
    }
}

void Display::SwitchScreenMode(bool hires) {
    hiresMode = hires;

    if (hiresMode) {
        currHeight = HIGH_HEIGHT;
        currWidth = HIGH_WIDTH;
    }else {
        currHeight = LOW_HEIGHT;
        currWidth = LOW_WIDTH;
    }
}

void Display::SetBitPlane(uint8_t newPlane) {
    bitPlane = newPlane;
}

std::array<uint8_t, 4> Display::GetPixelColor(uint8_t xPos, uint8_t yPos) const {
    if (displayBuffer1[yPos * currWidth + xPos] && displayBuffer2[yPos * currWidth + xPos])
        return plane3Color;
    else if (displayBuffer2[yPos * currWidth + xPos])
        return plane2Color;
    else if (displayBuffer1[yPos * currWidth + xPos])
        return plane1Color;
    else
        return plane0Color;
}

int Display::GetScreenWidth() const {
    return currWidth;
}

int Display::GetScreenHeight() const {
    return currHeight;
}