cmake_minimum_required(VERSION 4.2)
project(Chip_8Emu)

set(CMAKE_CXX_STANDARD 20)
find_package(SDL3 REQUIRED)

add_executable(Chip_8Emu main.cpp
        src/chip8/Chip8.cpp
        include/chip8/Chip8.h
        src/chip8/CPU.cpp
        include/chip8/CPU.h
        src/chip8/Memory.cpp
        include/chip8/Memory.h
        src/chip8/Display.cpp
        include/chip8/Display.h
        src/chip8/Keyboard.cpp
        include/chip8/Keyboard.h
        src/chip8/Timers.cpp
        include/chip8/Timers.h
        src/platform/SDLPlatform.cpp
        include/platform/SDLPlatform.h
)
target_link_libraries(Chip_8Emu PRIVATE SDL3::SDL3)
target_include_directories(Chip_8Emu PRIVATE include)