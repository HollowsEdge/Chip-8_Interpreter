package chip8

import kotlin.random.Random

class CPU(private val memory: Memory,
          private val display: Display,
          private val keyboard: Keyboard,
          private val soundTimer: Timer,
          private val delayTimer: Timer) {
    private var programCounter: Int = Memory.ROM_START_INDEX
    private var indexRegister: Int = 0
    private var registers: IntArray = IntArray(16)
    private var addressStack: ArrayDeque<Int> = ArrayDeque()

    val useModernShiftQuirk: Boolean = false 
    val useVFResetQuirk: Boolean = true
    val useIndexAddQuirk: Boolean = true
    val useMemoryQuirk: Boolean = true


    val mainTable: Array<(UShort) -> Unit> = arrayOf(
        ::op0nnn,
        ::op1nnn,
        ::op2nnn,
        ::op3xnn,
        ::op4xnn,
        ::op5xy0,
        ::op6xnn,
        ::op7xnn,
        ::op8xxx,
        ::op9xy0,
        ::opAnnn,
        ::opBnnn,
        ::opCxnn,
        ::opDxyn,
        ::opExxx,
        ::opFxxx 
    )
    val table8: Array<(UShort) -> Unit> = Array<(UShort) -> Unit>(16) { ::opInvalid }.apply {
            this[0x0] = ::op8xy0
            this[0x1] = ::op8xy1
            this[0x2] = ::op8xy2
            this[0x3] = ::op8xy3
            this[0x4] = ::op8xy4
            this[0x5] = ::op8xy5
            this[0x6] = ::op8xy6
            this[0x7] = ::op8xy7
            this[0xE] = ::op8xyE
        }
    val tableE: Array<(UShort) -> Unit> = Array<(UShort) -> Unit>(256) { ::opInvalid }.apply {
            this[0x9E] = ::opEx9E
            this[0xA1] = ::opExA1
        }
    val tableF: Array<(UShort) -> Unit> = Array<(UShort) -> Unit>(256) { ::opInvalid }.apply {
            this[0x07] = ::opFx07
            this[0x0A] = ::opFx0A
            this[0x15] = ::opFx15
            this[0x18] = ::opFx18
            this[0x1E] = ::opFx1E
            this[0x29] = ::opFx29
            this[0x33] = ::opFx33
            this[0x55] = ::opFx55
            this[0x65] = ::opFx65
        }

    fun cycle(){
        val opcode: UShort = fetchOpcode()
        decodeAndExecute(opcode)
    }
    
    fun fetchOpcode(): UShort {
        val highByte: UByte = memory.read(programCounter)
        val lowByte: UByte = memory.read(programCounter + 1)
        val opcode: Int = (highByte.toInt() shl 8) or lowByte.toInt()
        programCounter += 2
        return opcode.toUShort()
    }
    
    fun decodeAndExecute(opcode: UShort){
        val index = (opcode.toInt() and 0xF000) shr 12
        mainTable[index](opcode)
    }

    fun opInvalid(opCode: UShort){
        print("Tried to execute invalid opcode: ${opCode.toHexString()} \n")
    }

    fun op0nnn(opCode: UShort) {
        val nn: Int = opCode.toInt() and 0xFF
        when (nn) {
            0xE0 -> op00E0(opCode)
            0xEE -> op00EE(opCode)
            else -> print("Tried to execute invalid system opcode: ${opCode.toHexString()} \n")
        }
    }

    fun op00E0(opCode: UShort) {
        // Clear screen
        display.clear()
    }

    fun op00EE(opCode: UShort) {
        val targetAddress: Int? = addressStack.removeLastOrNull()
        if (targetAddress != null) 
            programCounter = targetAddress
    }

    fun op1nnn(opCode: UShort) {
        programCounter = opCode.toInt() and 0xFFF
    }

    fun op2nnn(opCode: UShort) {
        val nnn: Int = opCode.toInt() and 0xFFF
        addressStack.add(programCounter)
        programCounter = nnn
    }

    fun op3xnn(opCode: UShort) {
        val nn: Int = opCode.toInt() and 0xFF
        val x: Int = (opCode.toInt() and 0x0F00) shr 8
        if (registers[x] == nn)
            programCounter += 2
    }

    fun op4xnn(opCode: UShort) {
        val nn: Int = opCode.toInt() and 0xFF
        val x: Int = (opCode.toInt() and 0x0F00) shr 8
        if (registers[x] != nn)
            programCounter += 2
    }

    fun op5xy0(opCode: UShort) {
        val x: Int = (opCode.toInt() and 0x0F00) shr 8
        val y: Int = (opCode.toInt() and 0x00F0) shr 4
        if (registers[x] == registers[y])
            programCounter += 2
    }

    fun op6xnn(opCode: UShort) {
        val nn: Int = opCode.toInt() and 0xFF
        val x: Int = (opCode.toInt() and 0x0F00) shr 8
        registers[x] = nn
    }

    fun op7xnn(opCode: UShort) {
        val nn: Int = opCode.toInt() and 0xFF
        val x: Int = (opCode.toInt() and 0x0F00) shr 8
        registers[x] = (registers[x] + nn) and 0xFF
    }

    fun op8xxx(opcode: UShort) {
        val index = opcode.toInt() and 0xF
        table8[index](opcode)
    }

    fun op8xy0(opCode: UShort) {
        val x: Int = (opCode.toInt() and 0x0F00) shr 8
        val y: Int = (opCode.toInt() and 0x00F0) shr 4
        registers[x] = registers[y]
    }

    fun op8xy1(opCode: UShort) {
        val x: Int = (opCode.toInt() and 0x0F00) shr 8
        val y: Int = (opCode.toInt() and 0x00F0) shr 4
        registers[x] = registers[x] or registers[y]
        if (useVFResetQuirk)
            registers[0xF] = 0;
    }

    fun op8xy2(opCode: UShort) {
        val x: Int = (opCode.toInt() and 0x0F00) shr 8
        val y: Int = (opCode.toInt() and 0x00F0) shr 4
        registers[x] = registers[x] and registers[y]
        if (useVFResetQuirk)
            registers[0xF] = 0;
    }

    fun op8xy3(opCode: UShort) {
        val x: Int = (opCode.toInt() and 0x0F00) shr 8
        val y: Int = (opCode.toInt() and 0x00F0) shr 4
        registers[x] = registers[x] xor registers[y]
        if (useVFResetQuirk)
            registers[0xF] = 0;
    }

    fun op8xy4(opCode: UShort) {
        val x: Int = (opCode.toInt() and 0x0F00) shr 8
        val y: Int = (opCode.toInt() and 0x00F0) shr 4
        val totalValue: Int = registers[x] + registers[y]
        registers[x] = totalValue and 0xFF
        registers[0xF] = if (totalValue > 255) 1 else 0
    }

    fun op8xy5(opCode: UShort) {
        val x: Int = (opCode.toInt() and 0x0F00) shr 8
        val y: Int = (opCode.toInt() and 0x00F0) shr 4
        val operand1: Int = registers[x]
        registers[x] = (operand1 - registers[y]) and 0xFF
        registers[0xF] = if (operand1 >= registers[y]) 1 else 0
    }

    fun op8xy6(opCode: UShort) {
        val x: Int = (opCode.toInt() and 0x0F00) shr 8
        if (useModernShiftQuirk) {
            val operand1: Int = registers[x]
            registers[x] = registers[x] shr 1
            registers[0xF] = operand1 and 1
        } else {
            val y: Int = (opCode.toInt() and 0x00F0) shr 4
            registers[x] = registers[y] shr 1
            registers[0xF] = registers[y] and 1
        }
    }

    fun op8xy7(opCode: UShort) {
        val x: Int = (opCode.toInt() and 0x0F00) shr 8
        val y: Int = (opCode.toInt() and 0x00F0) shr 4
        val operand1: Int = registers[x]
        registers[x] = (registers[y] - operand1) and 0xFF
        registers[0xF] = if (registers[y] >= operand1) 1 else 0
    }
    
    fun op8xyE(opCode: UShort) {
        val x: Int = (opCode.toInt() and 0x0F00) shr 8
        if (useModernShiftQuirk) {
            val operand1: Int = registers[x]
            registers[x] = (registers[x] shl 1) and 0xFF
            registers[0xF] = (operand1 shr 7) and 1
        } else {
            val y: Int = (opCode.toInt() and 0x00F0) shr 4
            registers[x] = (registers[y] shl 1) and 0xFF
            registers[0xF] = (registers[y] shr 7) and 1
        }
    }

    fun op9xy0(opCode: UShort) {
        val x: Int = (opCode.toInt() and 0x0F00) shr 8
        val y: Int = (opCode.toInt() and 0x00F0) shr 4
        if (registers[x] != registers[y])
            programCounter += 2
    }

    fun opAnnn(opCode: UShort) {
        indexRegister = opCode.toInt() and 0xFFF
    }

    fun opBnnn(opCode: UShort) {
        val nnn = opCode.toInt() and 0xFFF
        programCounter = nnn + registers[0]
    }

    fun opCxnn(opCode: UShort) {
        val x = (opCode.toInt() and 0x0F00) shr 8
        val nn = opCode.toInt() and 0xFF

        registers[x] = Random.nextInt(0, 256) and nn
    }

    @OptIn(ExperimentalUnsignedTypes::class)
    fun opDxyn(opCode: UShort) {
        val x: Int = (opCode.toInt() and 0x0F00) shr 8
        val y: Int = (opCode.toInt() and 0x00F0) shr 4
        val n: Int = opCode.toInt() and 0xF

        val spriteData = UByteArray(n) { row ->
            memory.read(indexRegister + row)
        }
        
        val overlap: Boolean = display.drawSprite(
            registers[x],
            registers[y],
            n,
            spriteData
        )
        registers[0xF] = if (overlap) 1 else 0
    }

    fun opExxx(opcode: UShort) {
        val index = opcode.toInt() and 0xFF
        tableE[index](opcode)
    }

    fun opEx9E(opCode: UShort) {
        val x: Int = (opCode.toInt() and 0x0F00) shr 8
        if (keyboard.getKeyDown(registers[x] and 0xF))
            programCounter += 2
    }

    fun opExA1(opCode: UShort) {
        val x: Int = (opCode.toInt() and 0x0F00) shr 8
        if (!keyboard.getKeyDown(registers[x] and 0xF))
            programCounter += 2
    }

    fun opFxxx(opcode: UShort) {
        val index = opcode.toInt() and 0xFF
        tableF[index](opcode)
    }

    fun opFx07(opCode: UShort) {
        val x: Int = (opCode.toInt() and 0x0F00) shr 8
        registers[x] = delayTimer.getTime()
    }

    fun opFx0A(opCode: UShort) {
        val x: Int = (opCode.toInt() and 0x0F00) shr 8
        val key: Int = keyboard.getAnyKeyReleased()
        if (key == -1)
            programCounter -= 2
        else
            registers[x] = key
    }

    fun opFx15(opCode: UShort) {
        val x: Int = (opCode.toInt() and 0x0F00) shr 8
        delayTimer.setTimer(registers[x])
    }

    fun opFx18(opCode: UShort) {
        val x: Int = (opCode.toInt() and 0x0F00) shr 8
        soundTimer.setTimer(registers[x])
    }

    fun opFx1E(opCode: UShort) {
        val x: Int = (opCode.toInt() and 0x0F00) shr 8
        indexRegister += registers[x]
        if (useIndexAddQuirk)
            if (indexRegister >= 0x1000)
                registers[0xF] = 1;
    }

    fun opFx29(opCode: UShort) {
        val x: Int = (opCode.toInt() and 0x0F00) shr 8
        indexRegister = Memory.FONT_START_INDEX + (registers[x] * 5)
    }

    fun opFx33(opCode: UShort) {
        val x: Int = (opCode.toInt() and 0x0F00) shr 8
        var value: Int = registers[x]

        // Ones-place
        memory.write(indexRegister + 2, (value % 10).toUByte())
        value /= 10
        
        // Tens-place
        memory.write(indexRegister + 1, (value % 10).toUByte())
        value /= 10
        
        // Hundreds-place
        memory.write(indexRegister, (value % 10).toUByte())
    }

    fun opFx55(opCode: UShort) {
        val x: Int = (opCode.toInt() and 0x0F00) shr 8
        for (i in 0..x) {
            memory.write(indexRegister + i, registers[i].toUByte())
        }
        if (useMemoryQuirk) 
            indexRegister += x + 1
    }

    fun opFx65(opCode: UShort) {
        val x: Int = (opCode.toInt() and 0x0F00) shr 8
        for (i in 0..x) {
            registers[i] = memory.read(indexRegister + i).toInt()
        }
        if (useMemoryQuirk) 
            indexRegister += x + 1
    }
}