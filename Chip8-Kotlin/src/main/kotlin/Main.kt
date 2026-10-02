import chip8.Chip8;
import platform.Platform
import platform.SDLPlatform;

fun main() {
    val romPath: String = "romName.ch8"
    val initWindowWidth: Int = 1280
    val initWindowHeight: Int = 720
    
    val chip8: Chip8 = Chip8(romPath)
    val platform: Platform = SDLPlatform("Chip8 - ROM: $romPath", initWindowWidth, initWindowHeight)
    
    val cpuHz: Double = 1000.0
    val timerHz: Double = 60.0

    var running: Boolean = true
    var lastTime: Long = System.nanoTime()
    var deltaTime: Double = 0.0
    var currentTime: Long = System.nanoTime()

    var cpuAccumulator: Double = 0.0
    var timerAccumulator: Double = 0.0

    while (running)
    {
        currentTime = System.nanoTime()
        deltaTime = (currentTime - lastTime) / 1_000_000_000.0
        lastTime = currentTime

        cpuAccumulator += deltaTime
        timerAccumulator += deltaTime

        running = platform.processInput(chip8.keyboard)
        chip8.updateKeys()

        while (cpuAccumulator >= 1.0 / cpuHz)
        {
            chip8.cycle()
            cpuAccumulator -= 1.0 / cpuHz
        }

        while (timerAccumulator >= 1.0 / timerHz)
        {
            chip8.updateTimers()
            timerAccumulator -= 1.0 / timerHz
        }

        platform.render(chip8.display)
    }
}