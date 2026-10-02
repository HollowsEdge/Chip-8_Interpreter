package chip8
import java.io.File

@OptIn(ExperimentalUnsignedTypes::class)
class Chip8(romPath: String) {
    val display: Display = Display()
    val keyboard: Keyboard = Keyboard()
    val memory: Memory = Memory()
    val soundTimer: Timer = Timer()
    val delayTimer: Timer = Timer()


    val cpu: CPU = CPU(memory, display, keyboard, soundTimer, delayTimer)

    init {
        // Load Font
        val fontArray: UByteArray = ubyteArrayOf(
            0xF0u, 0x90u, 0x90u, 0x90u, 0xF0u, // 0
            0x20u, 0x60u, 0x20u, 0x20u, 0x70u, // 1
            0xF0u, 0x10u, 0xF0u, 0x80u, 0xF0u, // 2
            0xF0u, 0x10u, 0xF0u, 0x10u, 0xF0u, // 3
            0x90u, 0x90u, 0xF0u, 0x10u, 0x10u, // 4
            0xF0u, 0x80u, 0xF0u, 0x10u, 0xF0u, // 5
            0xF0u, 0x80u, 0xF0u, 0x90u, 0xF0u, // 6
            0xF0u, 0x10u, 0x20u, 0x40u, 0x40u, // 7
            0xF0u, 0x90u, 0xF0u, 0x90u, 0xF0u, // 8
            0xF0u, 0x90u, 0xF0u, 0x10u, 0xF0u, // 9
            0xF0u, 0x90u, 0xF0u, 0x90u, 0x90u, // A
            0xE0u, 0x90u, 0xE0u, 0x90u, 0xE0u, // B
            0xF0u, 0x80u, 0x80u, 0x80u, 0xF0u, // C
            0xE0u, 0x90u, 0x90u, 0x90u, 0xE0u, // D
            0xF0u, 0x80u, 0xF0u, 0x80u, 0xF0u, // E
            0xF0u, 0x80u, 0xF0u, 0x80u, 0x80u  // F
        )
        memory.load(Memory.FONT_START_INDEX, fontArray)
        
        // Load ROM
        val file = File(romPath)
        val bytes: ByteArray = file.readBytes()
        memory.load(Memory.ROM_START_INDEX, bytes.toUByteArray())
    }
    
    fun updateKeys() {
        keyboard.updateKeys()
    }
    
    fun updateTimers() {
        soundTimer.tickTimer()
        delayTimer.tickTimer()
    }
    
    fun cycle(){
        cpu.cycle()
    }
}