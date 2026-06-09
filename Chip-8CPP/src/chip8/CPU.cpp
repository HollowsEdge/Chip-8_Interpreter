#include "chip8/CPU.h"

#include <array>
#include <iostream>
#include <ostream>
#include <cstdlib>
#include <ctime>
#include <format>

#include "chip8/Chip8.h"

uint32_t CPU::FetchOpcode() {
    uint32_t opcode1 = (memory.GetMemory(programCounter) << 8) | memory.GetMemory(programCounter + 1);
    uint32_t opcode2 = 0;
    if (Chip8::selectedPlatform == Chip8::Platform::XO_Chip)
        opcode2 = (memory.GetMemory(programCounter + 2) << 8) | memory.GetMemory(programCounter + 3);
    uint32_t opCode4Bytes = (opcode1 << 16) | opcode2;
    // Even though we get 2 opcodes (4 bytes) only 1 instruction uses all 4. That instruction will handle the extra pc.
    programCounter += 2;
    return opCode4Bytes;
}

CPU::CPU(Memory &mem, Display &dis, Keyboard &key, Timers &timer)
    : memory(mem),
      display(dis),
      keyboard(key),
      timers(timer) {
    std::srand(std::time(nullptr));
    programCounter = Memory::romLoadStartAddress;

    mainTable[0x0] = &CPU::Table0;
    mainTable[0x1] = &CPU::OP_1NNN;
    mainTable[0x2] = &CPU::OP_2NNN;
    mainTable[0x3] = &CPU::OP_3XNN;
    mainTable[0x4] = &CPU::OP_4XNN;
    mainTable[0x5] = &CPU::Table5;
    mainTable[0x6] = &CPU::OP_6XNN;
    mainTable[0x7] = &CPU::OP_7XNN;
    mainTable[0x8] = &CPU::Table8;
    mainTable[0x9] = &CPU::OP_9XY0;
    mainTable[0xA] = &CPU::OP_ANNN;
    mainTable[0xB] = &CPU::OP_BNNN;
    mainTable[0xC] = &CPU::OP_CXNN;
    mainTable[0xD] = &CPU::TableD;
    mainTable[0xE] = &CPU::TableE;
    mainTable[0xF] = &CPU::TableF;

    // table0[0xCN] = &CPU::OP_00CN; // Special case delt with in function
    // table0[0xDN] = &CPU::OP_00DN; // Special case delt with in function
    table0[0xE0] = &CPU::OP_00E0;
    table0[0xEE] = &CPU::OP_00EE;
    table0[0xFB] = &CPU::OP_00FB;
    table0[0xFC] = &CPU::OP_00FC;
    table0[0xFD] = &CPU::OP_00FD;
    table0[0xFE] = &CPU::OP_00FE;
    table0[0xFF] = &CPU::OP_00FF;

    table5[0x0] = &CPU::OP_5XY0;
    table5[0x2] = &CPU::OP_5XY2;
    table5[0x3] = &CPU::OP_5XY3;

    table8[0x0] = &CPU::OP_8XY0;
    table8[0x1] = &CPU::OP_8XY1;
    table8[0x2] = &CPU::OP_8XY2;
    table8[0x3] = &CPU::OP_8XY3;
    table8[0x4] = &CPU::OP_8XY4;
    table8[0x5] = &CPU::OP_8XY5;
    table8[0x6] = &CPU::OP_8XY6;
    table8[0x7] = &CPU::OP_8XY7;
    table8[0xE] = &CPU::OP_8XYE;

    tableE[0x9E] = &CPU::OP_EX9E;
    tableE[0xA1] = &CPU::OP_EXA1;

    tableF[0x00] = &CPU::OP_F000;
    tableF[0x01] = &CPU::OP_FX01;
    tableF[0x02] = &CPU::OP_F002;
    tableF[0x07] = &CPU::OP_FX07;
    tableF[0x0A] = &CPU::OP_FX0A;
    tableF[0x15] = &CPU::OP_FX15;
    tableF[0x18] = &CPU::OP_FX18;
    tableF[0x1E] = &CPU::OP_FX1E;
    tableF[0x29] = &CPU::OP_FX29;
    tableF[0x30] = &CPU::OP_FX30;
    tableF[0x33] = &CPU::OP_FX33;
    tableF[0x3A] = &CPU::OP_FX3A;
    tableF[0x55] = &CPU::OP_FX55;
    tableF[0x65] = &CPU::OP_FX65;
    tableF[0x75] = &CPU::OP_FX75;
    tableF[0x85] = &CPU::OP_FX85;
}

void CPU::ExecuteOpcode(uint32_t doubleOpcode) {
    uint16_t opcode1 = doubleOpcode >> 16;
    uint8_t nibble = (opcode1 & 0xF000) >> 12;
    (this->*mainTable[nibble])(doubleOpcode);
}

void CPU::Table0(uint32_t doubleOpcode) {
    uint16_t opcode1 = doubleOpcode >> 16;
    switch (opcode1 & 0xF0) {
        case 0xC0:
            OP_00CN(doubleOpcode);
            break;
        case 0xD0:
            OP_00DN(doubleOpcode);
            break;
        default:
            uint8_t lastByte = opcode1 & 0xFF;
            (this->*table0[lastByte])(doubleOpcode);
            break;
    }
}

void CPU::Table5(uint32_t doubleOpcode) {
    uint16_t opcode1 = doubleOpcode >> 16;
    uint8_t lastNibble = opcode1 & 0xF;
    (this->*table5[lastNibble])(doubleOpcode);
}

void CPU::Table8(uint32_t doubleOpcode) {
    uint16_t opcode1 = doubleOpcode >> 16;
    uint8_t lastNibble = opcode1 & 0xF;
    (this->*table8[lastNibble])(doubleOpcode);
}

void CPU::TableD(uint32_t doubleOpcode) {
    uint16_t opcode1 = doubleOpcode >> 16;
    if ((opcode1 & 0xF) == 0x0 && Chip8::selectedPlatform != Chip8::Platform::Chip_8)
        OP_DXY0(doubleOpcode);
    else
        OP_DXYN(doubleOpcode);
}

void CPU::TableE(uint32_t doubleOpcode) {
    uint16_t opcode1 = doubleOpcode >> 16;
    uint8_t lastByte = opcode1 & 0xFF;
    (this->*tableE[lastByte])(doubleOpcode);
}

void CPU::TableF(uint32_t doubleOpcode) {
    uint16_t opcode1 = doubleOpcode >> 16;
    uint8_t lastByte = opcode1 & 0xFF;
    (this->*tableF[lastByte])(doubleOpcode);
}

// OpCodes

void CPU::OP_Invalid(uint32_t doubleOpcode) {
    uint16_t opcode1 = doubleOpcode >> 16;
    std::string hexString = std::format("{:#x}", opcode1);
    std::cerr << "Invalid opcode: " << hexString << std::endl;
}

void CPU::OP_1NNN(uint32_t doubleOpcode) {
    uint16_t opcode1 = doubleOpcode >> 16;
    programCounter = opcode1 & 0xFFF;
}

void CPU::OP_2NNN(uint32_t doubleOpcode) {
    uint16_t opcode1 = doubleOpcode >> 16;
    addressStack.push(programCounter);
    programCounter = opcode1 & 0xFFF;
}

void CPU::OP_3XNN(uint32_t doubleOpcode) {
    uint16_t opcode1 = doubleOpcode >> 16;
    uint16_t opcode2 = doubleOpcode & 0xFFFF;
    uint8_t targetRegister = (opcode1 & 0x0F00) >> 8;
    uint8_t targetValue = opcode1 & 0xFF;
    if (variableRegisters[targetRegister] == targetValue) {
        programCounter += 2;

        if (opcode2 == 0xF000) {
            if (Chip8::selectedPlatform != Chip8::Platform::XO_Chip) {
                std::string hexString = std::format("{:#x}", opcode2);
                std::cerr << "Skipping XO-Chip opcode while platform doesn't match. Results might be unexpected. opcode : " << hexString << std::endl;
            }
            else
                programCounter += 2;
        }
    }
}

void CPU::OP_4XNN(uint32_t doubleOpcode) {
    uint16_t opcode1 = doubleOpcode >> 16;
    uint16_t opcode2 = doubleOpcode & 0xFFFF;
    uint8_t targetRegister = (opcode1 & 0x0F00) >> 8;
    uint8_t targetValue = opcode1 & 0xFF;
    if (variableRegisters[targetRegister] != targetValue){
        programCounter += 2;

        if (opcode2 == 0xF000) {
            if (Chip8::selectedPlatform != Chip8::Platform::XO_Chip) {
                std::string hexString = std::format("{:#x}", opcode2);
                std::cerr << "Skipping XO-Chip opcode while platform doesn't match. Results might be unexpected. opcode : " << hexString << std::endl;
            }
            else
                programCounter += 2;
        }
    }
}

void CPU::OP_6XNN(uint32_t doubleOpcode) {
    uint16_t opcode1 = doubleOpcode >> 16;
    uint8_t targetRegister = (opcode1 & 0x0F00) >> 8;
    uint8_t lastByte = opcode1 & 0xFF;
    variableRegisters[targetRegister] = lastByte;
}

void CPU::OP_7XNN(uint32_t doubleOpcode) {
    uint16_t opcode1 = doubleOpcode >> 16;
    uint8_t targetRegister = (opcode1 & 0x0F00) >> 8;
    uint8_t lastByte = opcode1 & 0xFF;
    variableRegisters[targetRegister] += lastByte;
}

void CPU::OP_9XY0(uint32_t doubleOpcode) {
    uint16_t opcode1 = doubleOpcode >> 16;
    uint16_t opcode2 = doubleOpcode & 0xFFFF;
    uint8_t targetXRegister = (opcode1 & 0x0F00) >> 8;
    uint8_t targetYRegister = (opcode1 & 0x00F0) >> 4;
    if (variableRegisters[targetXRegister] != variableRegisters[targetYRegister]){
        programCounter += 2;

        if (opcode2 == 0xF000) {
            if (Chip8::selectedPlatform != Chip8::Platform::XO_Chip) {
                std::string hexString = std::format("{:#x}", opcode2);
                std::cerr << "Skipping XO-Chip opcode while platform doesn't match. Results might be unexpected. opcode : " << hexString << std::endl;
            }
            else
                programCounter += 2;
        }
    }
}

void CPU::OP_ANNN(uint32_t doubleOpcode) {
    uint16_t opcode1 = doubleOpcode >> 16;
    indexRegister = opcode1 & 0xFFF;
}

void CPU::OP_BNNN(uint32_t doubleOpcode) {
    uint16_t opcode1 = doubleOpcode >> 16;
    uint8_t targetXRegister = (opcode1 & 0x0F00) >> 8;
    programCounter = (opcode1 & 0xFFF) + variableRegisters[Chip8::selectedPlatform == Chip8::Platform::Super_Chip_8 ? targetXRegister : 0];
}

void CPU::OP_CXNN(uint32_t doubleOpcode) {
    uint16_t opcode1 = doubleOpcode >> 16;
    uint8_t targetXRegister = (opcode1 & 0x0F00) >> 8;
    variableRegisters[targetXRegister] = (std::rand() % 256) & (opcode1 & 0xFF);
}

void CPU::OP_DXYN(uint32_t doubleOpcode) {
    uint16_t opcode1 = doubleOpcode >> 16;
    uint8_t targetXRegister = (opcode1 & 0x0F00) >> 8;
    uint8_t targetYRegister = (opcode1 & 0x00F0) >> 4;
    uint8_t nibble = opcode1 & 0xF;
    bool overlap = display.DrawSprite(variableRegisters[targetXRegister], variableRegisters[targetYRegister], 8, nibble, memory.GetMemoryPtr(indexRegister));
    variableRegisters[0xF] = overlap ? 1 : 0;
}

void CPU::OP_DXY0(uint32_t doubleOpcode) {
    uint16_t opcode1 = doubleOpcode >> 16;
    if (Chip8::selectedPlatform == Chip8::Platform::Chip_8) {
        std::string hexString = std::format("{:#x}", opcode1);
        std::cerr <<
                "Executing Super_Chip8 or XO-Chip opcode while platform doesn't match. Results might be unexpected. opcode : "
                << hexString << std::endl;
    }

    uint8_t targetXRegister = (opcode1 & 0x0F00) >> 8;
    uint8_t targetYRegister = (opcode1 & 0x00F0) >> 4;
    bool overlap = display.DrawSprite(variableRegisters[targetXRegister], variableRegisters[targetYRegister], 16, 16,
                                      memory.GetMemoryPtr(indexRegister));
    variableRegisters[0xF] = overlap ? 1 : 0;
}

void CPU::OP_00CN(uint32_t doubleOpcode) {
    uint16_t opcode1 = doubleOpcode >> 16;
    if (Chip8::selectedPlatform == Chip8::Platform::Chip_8) {
        std::string hexString = std::format("{:#x}", opcode1);
        std::cerr <<
                   "Executing Super_Chip8 or XO-Chip opcode while platform doesn't match. Results might be unexpected. opcode : "
                   << hexString << std::endl;
    }

    uint8_t scrollPixels = opcode1 & 0xF;
    display.ScrollScreenDown(scrollPixels);
}

void CPU::OP_00DN(uint32_t doubleOpcode) {
    uint16_t opcode1 = doubleOpcode >> 16;
    if (Chip8::selectedPlatform != Chip8::Platform::XO_Chip) {
        std::string hexString = std::format("{:#x}", opcode1);
        std::cerr << "Executing XO-Chip opcode while platform doesn't match. Results might be unexpected. opcode : " <<
                hexString << std::endl;
    }
    uint8_t scrollPixels = opcode1 & 0xF;
    display.ScrollScreenUp(scrollPixels);
}

void CPU::OP_00E0(uint32_t doubleOpcode) {
    uint16_t opcode1 = doubleOpcode >> 16;
    display.Clear();
}

void CPU::OP_00EE(uint32_t doubleOpcode) {
    uint16_t opcode1 = doubleOpcode >> 16;
    programCounter = addressStack.top();
    addressStack.pop();
}

void CPU::OP_00FB(uint32_t doubleOpcode) {
    uint16_t opcode1 = doubleOpcode >> 16;
    if (Chip8::selectedPlatform == Chip8::Platform::Chip_8) {
        std::string hexString = std::format("{:#x}", opcode1);
        std::cerr <<
                "Executing Super_Chip8 or XO-Chip opcode while platform doesn't match. Results might be unexpected. opcode : "
                << hexString << std::endl;
    }
    display.ScrollScreenRight();
}

void CPU::OP_00FC(uint32_t doubleOpcode) {
    uint16_t opcode1 = doubleOpcode >> 16;
    if (Chip8::selectedPlatform == Chip8::Platform::Chip_8) {
        std::string hexString = std::format("{:#x}", opcode1);
        std::cerr <<
                "Executing Super_Chip8 or XO-Chip opcode while platform doesn't match. Results might be unexpected. opcode : "
                << hexString << std::endl;
    }
    display.ScrollScreenLeft();
}

void CPU::OP_00FD(uint32_t doubleOpcode) {
    uint16_t opcode1 = doubleOpcode >> 16;
    if (Chip8::selectedPlatform == Chip8::Platform::Chip_8) {
        std::string hexString = std::format("{:#x}", opcode1);
        std::cerr <<
                "Executing Super_Chip8 or XO-Chip opcode while platform doesn't match. Results might be unexpected. opcode : "
                << hexString << std::endl;
    }
    exitProgram = true;
}

void CPU::OP_00FE(uint32_t doubleOpcode) {
    uint16_t opcode1 = doubleOpcode >> 16;
    if (Chip8::selectedPlatform == Chip8::Platform::Chip_8) {
        std::string hexString = std::format("{:#x}", opcode1);
        std::cerr <<
                "Executing Super_Chip8 or XO-Chip opcode while platform doesn't match. Results might be unexpected. opcode : "
                << hexString << std::endl;
    }
    display.SwitchScreenMode(false);
}

void CPU::OP_00FF(uint32_t doubleOpcode) {
    uint16_t opcode1 = doubleOpcode >> 16;
    if (Chip8::selectedPlatform == Chip8::Platform::Chip_8) {
        std::string hexString = std::format("{:#x}", opcode1);
        std::cerr <<
                "Executing Super_Chip8 or XO-Chip opcode while platform doesn't match. Results might be unexpected. opcode : "
                << hexString << std::endl;
    }
    display.SwitchScreenMode(true);
}

void CPU::OP_5XY0(uint32_t doubleOpcode) {
    uint16_t opcode1 = doubleOpcode >> 16;
    uint16_t opcode2 = doubleOpcode & 0xFFFF;
    uint8_t targetXRegister = (opcode1 & 0x0F00) >> 8;
    uint8_t targetYRegister = (opcode1 & 0x00F0) >> 4;
    if (variableRegisters[targetXRegister] == variableRegisters[targetYRegister]){
        programCounter += 2;

        if (opcode2 == 0xF000) {
            if (Chip8::selectedPlatform != Chip8::Platform::XO_Chip) {
                std::string hexString = std::format("{:#x}", opcode2);
                std::cerr << "Skipping XO-Chip opcode while platform doesn't match. Results might be unexpected. opcode : " << hexString << std::endl;
            }
            else
                programCounter += 2;
        }
    }
}

void CPU::OP_5XY2(uint32_t doubleOpcode) {
    uint16_t opcode1 = doubleOpcode >> 16;
    if (Chip8::selectedPlatform != Chip8::Platform::XO_Chip) {
        std::string hexString = std::format("{:#x}", opcode1);
        std::cerr << "Executing XO-Chip opcode while platform doesn't match. Results might be unexpected. opcode : " <<
                hexString << std::endl;
    }
    uint8_t targetXRegister = (opcode1 & 0x0F00) >> 8;
    uint8_t targetYRegister = (opcode1 & 0x00F0) >> 4;
    int offset = 0;
    if (targetXRegister <= targetYRegister) {
        for (int i = targetXRegister; i <= targetYRegister; i++) {
            memory.SetMemory(indexRegister + offset++, variableRegisters[i]);
        }
    }else {
        for (int i = targetXRegister; i >= targetYRegister; i--) {
            memory.SetMemory(indexRegister + offset++, variableRegisters[i]);
        }
    }
}

void CPU::OP_5XY3(uint32_t doubleOpcode) {
    uint16_t opcode1 = doubleOpcode >> 16;
    if (Chip8::selectedPlatform != Chip8::Platform::XO_Chip) {
        std::string hexString = std::format("{:#x}", opcode1);
        std::cerr << "Executing XO-Chip opcode while platform doesn't match. Results might be unexpected. opcode : " <<
                hexString << std::endl;
    }
    uint8_t targetXRegister = (opcode1 & 0x0F00) >> 8;
    uint8_t targetYRegister = (opcode1 & 0x00F0) >> 4;
    int offset = 0;
    if (targetXRegister <= targetYRegister) {
        for (int i = targetXRegister; i <= targetYRegister; i++) {
            variableRegisters[i] = memory.GetMemory(indexRegister + offset++);
        }
    } else {
        for (int i = targetXRegister; i >= targetYRegister; i--) {
            variableRegisters[i] = memory.GetMemory(indexRegister + offset++);
        }
    }
}

void CPU::OP_8XY0(uint32_t doubleOpcode) {
    uint16_t opcode1 = doubleOpcode >> 16;
    uint8_t targetXRegister = (opcode1 & 0x0F00) >> 8;
    uint8_t targetYRegister = (opcode1 & 0x00F0) >> 4;
    variableRegisters[targetXRegister] = variableRegisters[targetYRegister];
}

void CPU::OP_8XY1(uint32_t doubleOpcode) {
    uint16_t opcode1 = doubleOpcode >> 16;
    uint8_t targetXRegister = (opcode1 & 0x0F00) >> 8;
    uint8_t targetYRegister = (opcode1 & 0x00F0) >> 4;
    variableRegisters[targetXRegister] |= variableRegisters[targetYRegister];
    if (useVFResetQuirk)
        variableRegisters[0xF] = 0;
}

void CPU::OP_8XY2(uint32_t doubleOpcode) {
    uint16_t opcode1 = doubleOpcode >> 16;
    uint8_t targetXRegister = (opcode1 & 0x0F00) >> 8;
    uint8_t targetYRegister = (opcode1 & 0x00F0) >> 4;
    variableRegisters[targetXRegister] &= variableRegisters[targetYRegister];
    if (useVFResetQuirk)
        variableRegisters[0xF] = 0;
}

void CPU::OP_8XY3(uint32_t doubleOpcode) {
    uint16_t opcode1 = doubleOpcode >> 16;
    uint8_t targetXRegister = (opcode1 & 0x0F00) >> 8;
    uint8_t targetYRegister = (opcode1 & 0x00F0) >> 4;
    variableRegisters[targetXRegister] ^= variableRegisters[targetYRegister];
    if (useVFResetQuirk)
        variableRegisters[0xF] = 0;
}

void CPU::OP_8XY4(uint32_t doubleOpcode) {
    uint16_t opcode1 = doubleOpcode >> 16;
    uint8_t targetXRegister = (opcode1 & 0x0F00) >> 8;
    uint8_t targetYRegister = (opcode1 & 0x00F0) >> 4;
    uint16_t totalValue = variableRegisters[targetXRegister] + variableRegisters[targetYRegister];
    variableRegisters[targetXRegister] = totalValue;
    variableRegisters[0xF] = totalValue > 255 ? 1 : 0;
}

void CPU::OP_8XY5(uint32_t doubleOpcode) {
    uint16_t opcode1 = doubleOpcode >> 16;
    uint8_t targetXRegister = (opcode1 & 0x0F00) >> 8;
    uint8_t targetYRegister = (opcode1 & 0x00F0) >> 4;
    uint8_t operand1 = variableRegisters[targetXRegister];
    variableRegisters[targetXRegister] = operand1 - variableRegisters[targetYRegister];
    variableRegisters[0xF] = (operand1 >= variableRegisters[targetYRegister]) ? 1 : 0;
}

void CPU::OP_8XY6(uint32_t doubleOpcode) {
    uint16_t opcode1 = doubleOpcode >> 16;
    uint8_t targetXRegister = (opcode1 & 0x0F00) >> 8;
    if (useModernShiftQuirk) {
        uint8_t operand1 = variableRegisters[targetXRegister];
        variableRegisters[targetXRegister] >>= 1;
        variableRegisters[0xF] = operand1 & 1;
    } else {
        uint8_t targetYRegister = (opcode1 & 0x00F0) >> 4;
        variableRegisters[targetXRegister] = variableRegisters[targetYRegister] >> 1;
        variableRegisters[0xF] = variableRegisters[targetYRegister] & 1;
    }
}

void CPU::OP_8XY7(uint32_t doubleOpcode) {
    uint16_t opcode1 = doubleOpcode >> 16;
    uint8_t targetXRegister = (opcode1 & 0x0F00) >> 8;
    uint8_t targetYRegister = (opcode1 & 0x00F0) >> 4;
    uint8_t operand1 = variableRegisters[targetXRegister];
    variableRegisters[targetXRegister] = variableRegisters[targetYRegister] - operand1;
    variableRegisters[0xF] = (variableRegisters[targetYRegister] >= operand1) ? 1 : 0;
}

void CPU::OP_8XYE(uint32_t doubleOpcode) {
    uint16_t opcode1 = doubleOpcode >> 16;
    uint8_t targetXRegister = (opcode1 & 0x0F00) >> 8;
    if (useModernShiftQuirk) {
        uint8_t operand1 = variableRegisters[targetXRegister];
        variableRegisters[targetXRegister] <<= 1;
        variableRegisters[0xF] = (operand1 >> 7) & 1;
    } else {
        uint8_t targetYRegister = (opcode1 & 0x00F0) >> 4;
        variableRegisters[targetXRegister] = variableRegisters[targetYRegister] << 1;
        variableRegisters[0xF] = (variableRegisters[targetYRegister] >> 7) & 1;
    }
}

void CPU::OP_EX9E(uint32_t doubleOpcode) {
    uint16_t opcode1 = doubleOpcode >> 16;
    uint16_t opcode2 = doubleOpcode & 0xFFFF;
    uint8_t targetXRegister = (opcode1 & 0x0F00) >> 8;
    if (keyboard.IsKeyDown(variableRegisters[targetXRegister])) {
        programCounter += 2;

        if (opcode2 == 0xF000) {
            if (Chip8::selectedPlatform != Chip8::Platform::XO_Chip) {
                std::string hexString = std::format("{:#x}", opcode2);
                std::cerr << "Skipping XO-Chip opcode while platform doesn't match. Results might be unexpected. opcode : " << hexString << std::endl;
            }
            else
                programCounter += 2;
        }
    }
}

void CPU::OP_EXA1(uint32_t doubleOpcode) {
    uint16_t opcode1 = doubleOpcode >> 16;
    uint16_t opcode2 = doubleOpcode & 0xFFFF;
    uint8_t targetXRegister = (opcode1 & 0x0F00) >> 8;
    if (!keyboard.IsKeyDown(variableRegisters[targetXRegister])) {
        programCounter += 2;

        if (opcode2 == 0xF000) {
            if (Chip8::selectedPlatform != Chip8::Platform::XO_Chip) {
                std::string hexString = std::format("{:#x}", opcode2);
                std::cerr << "Skipping XO-Chip opcode while platform doesn't match. Results might be unexpected. opcode : " << hexString << std::endl;
            }
            else
                programCounter += 2;
        }
    }
}

void CPU::OP_F000(uint32_t doubleOpcode) {
    uint16_t opcode1 = doubleOpcode >> 16;
    if (Chip8::selectedPlatform != Chip8::Platform::XO_Chip) {
        std::string hexString = std::format("{:#x}", opcode1);
        std::cerr << "Executing XO-Chip opcode while platform doesn't match. Results might be unexpected. opcode : " << hexString << std::endl;
    }
    indexRegister = doubleOpcode & 0xFFFF;
    programCounter += 2; // Used a double wide instruction
}

void CPU::OP_FX01(uint32_t doubleOpcode) {
    uint16_t opcode1 = doubleOpcode >> 16;
    if (Chip8::selectedPlatform != Chip8::Platform::XO_Chip) {
        std::string hexString = std::format("{:#x}", opcode1);
        std::cerr << "Executing XO-Chip opcode while platform doesn't match. Results might be unexpected. opcode : " <<
                hexString << std::endl;
    }
    uint8_t drawingPlanes = (opcode1 & 0x0F00) >> 8;
    display.SetBitPlane(drawingPlanes);
}

void CPU::OP_F002(uint32_t doubleOpcode) {
    uint16_t opcode1 = doubleOpcode >> 16;
    if (Chip8::selectedPlatform != Chip8::Platform::XO_Chip) {
        std::string hexString = std::format("{:#x}", opcode1);
        std::cerr << "Executing XO-Chip opcode while platform doesn't match. Results might be unexpected. opcode : " <<
                hexString << std::endl;
    }
    std::string hexString = std::format("{:#x}", opcode1);
    std::cerr << "Uninplemented opcode: " << hexString << std::endl;
}

void CPU::OP_FX07(uint32_t doubleOpcode) {
    uint16_t opcode1 = doubleOpcode >> 16;
    uint8_t targetXRegister = (opcode1 & 0x0F00) >> 8;
    variableRegisters[targetXRegister] = timers.GetDelayTimerTime();
}

void CPU::OP_FX0A(uint32_t doubleOpcode) {
    uint16_t opcode1 = doubleOpcode >> 16;
    uint8_t targetXRegister = (opcode1 & 0x0F00) >> 8;
    int keyReleased = keyboard.GetFirstKeyJustReleased();
    if (keyReleased == -1) {
        programCounter -= 2;
    } else {
        variableRegisters[targetXRegister] = keyReleased;
    }
}

void CPU::OP_FX15(uint32_t doubleOpcode) {
    uint16_t opcode1 = doubleOpcode >> 16;
    uint8_t targetXRegister = (opcode1 & 0x0F00) >> 8;
    timers.SetDelayTimerTime(variableRegisters[targetXRegister]);
}

void CPU::OP_FX18(uint32_t doubleOpcode) {
    uint16_t opcode1 = doubleOpcode >> 16;
    uint8_t targetXRegister = (opcode1 & 0x0F00) >> 8;
    timers.SetSoundTimerTime(variableRegisters[targetXRegister]);
}

void CPU::OP_FX1E(uint32_t doubleOpcode) {
    uint16_t opcode1 = doubleOpcode >> 16;
    uint8_t targetXRegister = (opcode1 & 0x0F00) >> 8;
    indexRegister += variableRegisters[targetXRegister];
    if (useIndexAddQuirk)
        if (indexRegister >= 0x1000)
            variableRegisters[0xF] = 1;
}

void CPU::OP_FX29(uint32_t doubleOpcode) {
    uint16_t opcode1 = doubleOpcode >> 16;
    uint8_t targetXRegister = (opcode1 & 0x0F00) >> 8;
    // indexRegister = Memory::fontLoadStartAddress + ((variableRegisters[targetXRegister] & 0x0F) * 5);
    indexRegister = Memory::fontLoadStartAddress + (variableRegisters[targetXRegister] * 5);
}

void CPU::OP_FX30(uint32_t doubleOpcode) {
    uint16_t opcode1 = doubleOpcode >> 16;
    if (Chip8::selectedPlatform == Chip8::Platform::Chip_8) {
        std::string hexString = std::format("{:#x}", opcode1);
        std::cerr <<
                "Executing Super_Chip8 or XO-Chip opcode while platform doesn't match. Results might be unexpected. opcode : "
                << hexString << std::endl;
    }
    uint8_t targetXRegister = (opcode1 & 0x0F00) >> 8;
    // indexRegister = Memory::hiresFontLoadStartAddress + ((variableRegisters[targetXRegister] & 0x0F) * 10);
    indexRegister = Memory::hiresFontLoadStartAddress + (variableRegisters[targetXRegister] * 10);
}

void CPU::OP_FX33(uint32_t doubleOpcode) {
    uint16_t opcode1 = doubleOpcode >> 16;
    uint8_t targetXRegister = (opcode1 & 0x0F00) >> 8;
    int value = variableRegisters[targetXRegister];
    // Ones-place
    memory.SetMemory(indexRegister + 2, value % 10);
    value /= 10;

    // Tens-place
    memory.SetMemory(indexRegister + 1, value % 10);
    value /= 10;

    // Hundreds-place
    memory.SetMemory(indexRegister, value % 10);
}

void CPU::OP_FX3A(uint32_t doubleOpcode) {
    uint16_t opcode1 = doubleOpcode >> 16;
    if (Chip8::selectedPlatform != Chip8::Platform::XO_Chip) {
        std::string hexString = std::format("{:#x}", opcode1);
        std::cerr << "Executing XO-Chip opcode while platform doesn't match. Results might be unexpected. opcode : " <<
                hexString << std::endl;
    }
    std::string hexString = std::format("{:#x}", opcode1);
    std::cerr << "Uninplemented opcode: " << hexString << std::endl;
}

void CPU::OP_FX55(uint32_t doubleOpcode) {
    uint16_t opcode1 = doubleOpcode >> 16;
    uint8_t targetXRegister = (opcode1 & 0x0F00) >> 8;
    for (int i = 0; i <= targetXRegister; i++) {
        memory.SetMemory(indexRegister + i, variableRegisters[i]);
    }
    if (useMemoryQuirk)
        indexRegister += targetXRegister + 1;
}

void CPU::OP_FX65(uint32_t doubleOpcode) {
    uint16_t opcode1 = doubleOpcode >> 16;
    uint8_t targetXRegister = (opcode1 & 0x0F00) >> 8;
    for (int i = 0; i <= targetXRegister; i++) {
        variableRegisters[i] = memory.GetMemory(indexRegister + i);
    }
    if (useMemoryQuirk)
        indexRegister += targetXRegister + 1;
}

void CPU::OP_FX75(uint32_t doubleOpcode) {
    uint16_t opcode1 = doubleOpcode >> 16;
    if (Chip8::selectedPlatform == Chip8::Platform::Chip_8) {
        std::string hexString = std::format("{:#x}", opcode1);
        std::cerr <<
                "Executing Super_Chip8 or XO-Chip opcode while platform doesn't match. Results might be unexpected. opcode : "
                << hexString << std::endl;
    }

    // TODO: these should save to file instead of ram
    uint8_t targetXRegister = (opcode1 & 0x0F00) >> 8;
    for (int i = 0; i <= targetXRegister; i++)
        memory.SetFlagStorage(i, variableRegisters[i]);
}

void CPU::OP_FX85(uint32_t doubleOpcode) {
    uint16_t opcode1 = doubleOpcode >> 16;
    if (Chip8::selectedPlatform == Chip8::Platform::Chip_8) {
        std::string hexString = std::format("{:#x}", opcode1);
        std::cerr <<
                "Executing Super_Chip8 or XO-Chip opcode while platform doesn't match. Results might be unexpected. opcode : "
                << hexString << std::endl;
    }

    // TODO: these should save to file instead of ram
    uint8_t targetXRegister = (opcode1 & 0x0F00) >> 8;
    for (int i = 0; i <= targetXRegister; i++)
        variableRegisters[i] = memory.GetFlagStorage(i);
}
