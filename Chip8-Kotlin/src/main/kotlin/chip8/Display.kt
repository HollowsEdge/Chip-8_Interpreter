package chip8

class Display {
    companion object {
        const val WIDTH = 64
        const val HEIGHT = 32
    }

    private val displayBuffer: BooleanArray = BooleanArray(WIDTH * HEIGHT)

    fun clear() {
        displayBuffer.fill(false)
    }

    fun getPixel(x: Int, y: Int): Boolean {
        return displayBuffer[y * WIDTH + x]
    }

    fun setPixel(x: Int, y: Int, value: Boolean) {
        displayBuffer[y * WIDTH + x] = value
    }

    fun togglePixel(x: Int, y: Int) {
        val index = y * WIDTH + x
        displayBuffer[index] = !displayBuffer[index]
    }

    fun getPixels(): BooleanArray {
        return displayBuffer.copyOf()
    }

    @OptIn(ExperimentalUnsignedTypes::class)
    fun drawSprite(posX: Int, posY: Int, spriteHeight: Int, spriteData: UByteArray): Boolean {
        var overlap = false

        val startX = posX % WIDTH
        val startY = posY % HEIGHT

        for (row in 0 until spriteHeight) {
            val spriteByte = spriteData[row].toInt() and 0xFF

            for (col in 0 until 8) {
                val x = startX + col
                val y = startY + row

                if(x >= WIDTH || y >= HEIGHT)
                    continue
                
                val pixel = (spriteByte shr (7 - col)) and 1

                val index = y * WIDTH + x

                if (displayBuffer[index] && pixel == 1)
                    overlap = true
                
                displayBuffer[index] = (displayBuffer[index] xor (pixel == 1))
            }
        }
        return overlap
    }
}
