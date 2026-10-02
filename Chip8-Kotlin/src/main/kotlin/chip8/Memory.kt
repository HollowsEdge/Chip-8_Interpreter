package chip8

@OptIn(ExperimentalUnsignedTypes::class)
class Memory {
    companion object {
        const val SIZE = 4096
        const val FONT_START_INDEX = 0x50
        const val ROM_START_INDEX = 0x200
    }
    
    private val mem: UByteArray = UByteArray(SIZE)

    fun load(startIndex: Int, newValues: UByteArray) {
        newValues.copyInto(mem, destinationOffset = startIndex)
    }

    fun write(index: Int, newValue: UByte) {
        mem[index] = newValue
    }

    fun read(index: Int): UByte {
        return mem[index]
    }
}