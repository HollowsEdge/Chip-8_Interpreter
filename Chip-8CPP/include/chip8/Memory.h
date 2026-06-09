#pragma once

#include <cstdint>
#include <array>
#import <vector>

class Memory {
public:
    constexpr static uint16_t romLoadStartAddress = 512;
    constexpr static uint8_t fontLoadStartAddress= 80;
    constexpr static uint8_t hiresFontLoadStartAddress= 160;
    void SetMemory(uint16_t address, uint8_t value);
    int GetMemory(uint16_t address);
    void SetFlagStorage(uint8_t index, uint8_t value);
    int GetFlagStorage(uint8_t index);
    uint8_t* GetMemoryPtr(uint16_t address);
    void LoadRom(std::vector<uint8_t> romData);
    void LoadFont();
private:
    std::array<uint8_t, 65535> memory{0}; // Chip8 only needs 4096 but XO-Chip needs more. Chip8 doesn't access this extra space.
    std::array<uint8_t, 16> flagsStorage{0};
};