#pragma once

#include "chip8/Memory.h"
#include "chip8/Display.h"
#include "chip8/Keyboard.h"
#include "chip8/Timers.h"
#include <cstdint>
#include <stack>

class CPU {
    public:
        CPU(Memory& mem, Display& dis, Keyboard& key, Timers& timer);
        using OpFunc = void(CPU::*)(uint32_t);
        void ExecuteOpcode(uint32_t doubleOpcode);
        void Table0(uint32_t doubleOpcode);
        void Table5(uint32_t doubleOpcode);
        void Table8(uint32_t doubleOpcode);
        void TableD(uint32_t doubleOpcode);
        void TableE(uint32_t doubleOpcode);
        void TableF(uint32_t doubleOpcode);
        OpFunc mainTable[16]{&CPU::OP_Invalid};
        OpFunc table0[0xFF + 1]{&CPU::OP_Invalid};
        OpFunc table5[0x3 + 1]{&CPU::OP_Invalid};
        OpFunc table8[0xE + 1]{&CPU::OP_Invalid};
        OpFunc tableE[0xA1 + 1]{&CPU::OP_Invalid};
        OpFunc tableF[0x85 + 1]{&CPU::OP_Invalid};
        void OP_Invalid(uint32_t doubleOpcode);
        void OP_1NNN(uint32_t doubleOpcode);
        void OP_2NNN(uint32_t doubleOpcode);
        void OP_3XNN(uint32_t doubleOpcode);
        void OP_4XNN(uint32_t doubleOpcode);
        void OP_6XNN(uint32_t doubleOpcode);
        void OP_7XNN(uint32_t doubleOpcode);
        void OP_9XY0(uint32_t doubleOpcode);
        void OP_ANNN(uint32_t doubleOpcode);
        void OP_BNNN(uint32_t doubleOpcode);
        void OP_CXNN(uint32_t doubleOpcode);
        void OP_DXYN(uint32_t doubleOpcode);
        void OP_DXY0(uint32_t doubleOpcode);
        void OP_00CN(uint32_t doubleOpcode);
        void OP_00DN(uint32_t doubleOpcode);
        void OP_00E0(uint32_t doubleOpcode);
        void OP_00EE(uint32_t doubleOpcode);
        void OP_00FB(uint32_t doubleOpcode);
        void OP_00FC(uint32_t doubleOpcode);
        void OP_00FD(uint32_t doubleOpcode);
        void OP_00FE(uint32_t doubleOpcode);
        void OP_00FF(uint32_t doubleOpcode);
        void OP_5XY0(uint32_t doubleOpcode);
        void OP_5XY2(uint32_t doubleOpcode);
        void OP_5XY3(uint32_t doubleOpcode);
        void OP_8XY0(uint32_t doubleOpcode);
        void OP_8XY1(uint32_t doubleOpcode);
        void OP_8XY2(uint32_t doubleOpcode);
        void OP_8XY3(uint32_t doubleOpcode);
        void OP_8XY4(uint32_t doubleOpcode);
        void OP_8XY5(uint32_t doubleOpcode);
        void OP_8XY6(uint32_t doubleOpcode);
        void OP_8XY7(uint32_t doubleOpcode);
        void OP_8XYE(uint32_t doubleOpcode);
        void OP_EX9E(uint32_t doubleOpcode);
        void OP_EXA1(uint32_t doubleOpcode);
        void OP_F000(uint32_t doubleOpcode);
        void OP_FX01(uint32_t doubleOpcode);
        void OP_F002(uint32_t doubleOpcode);
        void OP_FX07(uint32_t doubleOpcode);
        void OP_FX0A(uint32_t doubleOpcode);
        void OP_FX15(uint32_t doubleOpcode);
        void OP_FX18(uint32_t doubleOpcode);
        void OP_FX1E(uint32_t doubleOpcode);
        void OP_FX29(uint32_t doubleOpcode);
        void OP_FX30(uint32_t doubleOpcode);
        void OP_FX33(uint32_t doubleOpcode);
        void OP_FX3A(uint32_t doubleOpcode);
        void OP_FX55(uint32_t doubleOpcode);
        void OP_FX65(uint32_t doubleOpcode);
        void OP_FX75(uint32_t doubleOpcode);
        void OP_FX85(uint32_t doubleOpcode);
        uint32_t FetchOpcode();
        bool exitProgram = false;
    private:
        Memory& memory;
        Display& display;
        Keyboard& keyboard;
        Timers& timers;

        uint16_t programCounter = 0;
        uint16_t indexRegister = 0;
        std::stack<uint16_t> addressStack{};
        std::array<uint8_t, 16> variableRegisters{0};

        // Quirks
        // XO-Chip
        bool useModernShiftQuirk = false;
        bool useVFResetQuirk = false;
        bool useIndexAddQuirk = true;
        bool useMemoryQuirk = true;

        // Super-Chip
        // bool useModernShiftQuirk = true;
        // bool useVFResetQuirk = false;
        // bool useIndexAddQuirk = true;
        // bool useMemoryQuirk = false;

        // Chip8
        // bool useModernShiftQuirk = true;
        // bool useVFResetQuirk = true;
        // bool useIndexAddQuirk = true;
        // bool useMemoryQuirk = true;
};

