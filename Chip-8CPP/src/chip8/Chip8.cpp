#include "chip8/Chip8.h"
#include <string>
#include <fstream>
#include <iterator>
#include <vector>
#include <cstdint>
#include <filesystem>
#include <iostream>

Chip8::Chip8() : keyboard(), memory(), display(), timers(), cpu(memory, display, keyboard, timers) {}

void Chip8::LoadROM(const std::string& path) {
    std::ifstream inFile(path, std::ios::binary);

    if (!inFile) {
        throw std::runtime_error("Failed to open ROM: " + path);
    }

    std::vector<uint8_t> bytes(
         (std::istreambuf_iterator<char>(inFile)),
         (std::istreambuf_iterator<char>()));

    memory.LoadRom(bytes);
    memory.LoadFont();
}

void Chip8::Cycle() {
    // Fetch opcode
    uint32_t doubleOpCode = cpu.FetchOpcode();
    // Decode and Execute instruction
    cpu.ExecuteOpcode(doubleOpCode);
}

void Chip8::UpdateTimers() {
    // Decrease delay and sound timers (60 Hz)
    timers.UpdateTimers();
}

Display& Chip8::GetDisplay() {
    return display;
}

bool Chip8::GetExitProgram() {
    return cpu.exitProgram;
}