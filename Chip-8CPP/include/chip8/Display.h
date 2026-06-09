#pragma once

#include <array>
#include <cstdint>

class Display {
    public:
        static constexpr int LOW_WIDTH  = 64;
        static constexpr int LOW_HEIGHT = 32;

        static constexpr int HIGH_WIDTH  = 128;
        static constexpr int HIGH_HEIGHT = 64;

        int currWidth  = LOW_WIDTH;
        int currHeight = LOW_HEIGHT;

        uint8_t* GetDisplayBuffer(uint8_t buffer);
        bool DrawSprite(int posX, int posY, int spriteWidth, int spriteHeight, uint8_t* spriteData);
        void Clear();
        void ScrollScreenUp(uint8_t pixelAmount);
        void ScrollScreenDown(uint8_t pixelAmount);
        void ScrollScreenLeft();
        void ScrollScreenRight();
        void SwitchScreenMode(bool hires);
        void SetBitPlane(uint8_t newPlane);
        int GetScreenWidth() const;
        int GetScreenHeight() const;
        std::array<uint8_t, 4> GetPixelColor(uint8_t xPos, uint8_t yPos) const;
    private:
        std::array<uint8_t, 4> plane0Color{ 153, 102, 0, 255 };
        std::array<uint8_t, 4> plane1Color{ 255, 204, 0, 255 };
        std::array<uint8_t, 4> plane3Color{ 102, 34, 0, 255 };
        std::array<uint8_t, 4> plane2Color{ 255, 102, 0, 255 };
        bool hiresMode{false};
        uint8_t bitPlane{1};
    std::array<uint8_t, HIGH_WIDTH * HIGH_HEIGHT> displayBuffer1{0};
    std::array<uint8_t, HIGH_WIDTH * HIGH_HEIGHT> displayBuffer2{0};
};
