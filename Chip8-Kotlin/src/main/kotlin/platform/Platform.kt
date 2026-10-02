package platform
import chip8.Keyboard
import chip8.Display

interface Platform {
    fun processInput(keyboard: Keyboard): Boolean
    fun render(display: Display)
    fun shutdown()
}