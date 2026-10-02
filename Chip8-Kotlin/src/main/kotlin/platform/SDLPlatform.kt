package platform

import chip8.Display
import chip8.Keyboard
import org.lwjgl.sdl.SDL
import org.lwjgl.sdl.SDL_Event
import org.lwjgl.sdl.SDL_FRect
import org.lwjgl.sdl.SDLInit.*
import org.lwjgl.sdl.SDLRender.*
import org.lwjgl.sdl.SDLScancode.*
import org.lwjgl.sdl.SDLEvents.*
import org.lwjgl.sdl.SDLError.*
import org.lwjgl.sdl.SDLPixels.SDL_PIXELFORMAT_RGBA8888
import org.lwjgl.sdl.SDLSurface.SDL_SCALEMODE_NEAREST
import org.lwjgl.sdl.SDLVideo.*
import org.lwjgl.system.MemoryStack
import org.lwjgl.sdl.SDL_PixelFormatDetails.*
import org.lwjgl.sdl.SDL_Texture
import org.lwjgl.sdl.SDL_Texture.*


import kotlin.math.floor
import kotlin.math.min

class SDLPlatform(
    windowTitle: String,
    windowWidth: Int,
    windowHeight: Int
) : Platform {

    companion object {
        private const val MINIMUM_WINDOW_WIDTH = 320
        private const val MINIMUM_WINDOW_HEIGHT = 240

        private const val PIXEL_ON_R: Byte = 255.toByte()
        private const val PIXEL_ON_G: Byte = 255.toByte()
        private const val PIXEL_ON_B: Byte = 255.toByte()

        private const val PIXEL_OFF_R: Byte = 0
        private const val PIXEL_OFF_G: Byte = 0
        private const val PIXEL_OFF_B: Byte = 0

        private const val ALPHA: Byte = 255.toByte()

        private const val WINDOW_FLAGS = 0L
    }

    private var window: Long = 0L
    private var renderer: Long = 0L
    private var texture: SDL_Texture? = null



    private val keyMap = mapOf(
        SDL_SCANCODE_1 to 0x1,
        SDL_SCANCODE_2 to 0x2,
        SDL_SCANCODE_3 to 0x3,
        SDL_SCANCODE_4 to 0xC,

        SDL_SCANCODE_Q to 0x4,
        SDL_SCANCODE_W to 0x5,
        SDL_SCANCODE_E to 0x6,
        SDL_SCANCODE_R to 0xD,

        SDL_SCANCODE_A to 0x7,
        SDL_SCANCODE_S to 0x8,
        SDL_SCANCODE_D to 0x9,
        SDL_SCANCODE_F to 0xE,

        SDL_SCANCODE_Z to 0xA,
        SDL_SCANCODE_X to 0x0,
        SDL_SCANCODE_C to 0xB,
        SDL_SCANCODE_V to 0xF
    )

    init {
        if (!SDL_Init(SDL_INIT_VIDEO)) {
            error("SDL_Init failed: ${SDL_GetError()}")
        }

        window = SDL_CreateWindow(
            windowTitle,
            windowWidth,
            windowHeight,
            WINDOW_FLAGS
        )

        if (window == 0L) {
            val errorMessage = SDL_GetError()

            SDL_Quit()

            error("Window creation failed: $errorMessage")
        }

        renderer = SDL_CreateRenderer(window, "")

        if (renderer == 0L) {
            val errorMessage = SDL_GetError()

            SDL_DestroyWindow(window)
            SDL_Quit()

            error("Renderer creation failed: $errorMessage")
        }

        texture = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_STREAMING,
            Display.WIDTH,
            Display.HEIGHT
        )

        if (texture == null) {
            val errorMessage = SDL_GetError()

            SDL_DestroyRenderer(renderer)
            SDL_DestroyWindow(window)
            SDL_Quit()

            error("Texture creation failed: $errorMessage")
        }

        SDL_SetTextureScaleMode(
            texture!!,
            SDL_SCALEMODE_NEAREST
        )

        SDL_SetWindowMinimumSize(
            window,
            MINIMUM_WINDOW_WIDTH,
            MINIMUM_WINDOW_HEIGHT
        )

        SDL_SetRenderVSync(renderer, 1)

        println("SDL initialized!")
    }

    override fun processInput(keyboard: Keyboard): Boolean {
        val event = SDL_Event.malloc()

        try {
            while (SDL_PollEvent(event)) {

                when (event.type()) {

                    SDL_EVENT_QUIT -> {
                        return false
                    }

                    SDL_EVENT_KEY_DOWN -> {
                        val key = keyMap[event.key().scancode()]

                        if (key != null) {
                            keyboard.setKey(key, true)
                        }
                    }

                    SDL_EVENT_KEY_UP -> {
                        val key = keyMap[event.key().scancode()]

                        if (key != null) {
                            keyboard.setKey(key, false)
                        }
                    }
                }
            }
        } finally {
            event.free()
        }

        return true
    }

    override fun render(display: Display) {
        val texture = texture ?: return

        MemoryStack.stackPush().use { stack ->

            // 4 bytes per pixel.
            val pixels = stack.malloc(
                Display.WIDTH * Display.HEIGHT * 4
            )

            for (y in 0 until Display.HEIGHT) {
                for (x in 0 until Display.WIDTH) {

                    val color: Byte =
                        if (display.getPixel(x, y)) {
                            0xFF.toByte()
                        } else {
                            0x00.toByte()
                        }

                    // ARGB8888
                    pixels.put(0xFF.toByte()) // A
                    pixels.put(color)         // R
                    pixels.put(color)         // G
                    pixels.put(color)         // B
                }
            }

            pixels.flip()

            SDL_UpdateTexture(
                texture,
                null,
                pixels,
                Display.WIDTH * 4
            )

            val widthBuffer = stack.mallocInt(1)
            val heightBuffer = stack.mallocInt(1)

            SDL_GetRenderOutputSize(
                renderer,
                widthBuffer,
                heightBuffer
            )

            val outputWidth = widthBuffer[0]
            val outputHeight = heightBuffer[0]

            // CHIP-8 = 64x32 = 2:1
            val scale = min(
                outputWidth / Display.WIDTH,
                outputHeight / Display.HEIGHT
            )

            if (scale <= 0) {
                return@use
            }

            val destinationWidth = Display.WIDTH * scale
            val destinationHeight = Display.HEIGHT * scale

            val offsetX = (outputWidth - destinationWidth) / 2
            val offsetY = (outputHeight - destinationHeight) / 2

            SDL_SetRenderDrawColor(
                renderer,
                0,
                0,
                0,
                255.toByte()
            )

            SDL_RenderClear(renderer)

            val destination = SDL_FRect.malloc(stack)

            destination.x(offsetX.toFloat())
            destination.y(offsetY.toFloat())
            destination.w(destinationWidth.toFloat())
            destination.h(destinationHeight.toFloat())

            SDL_RenderTexture(
                renderer,
                texture,
                null,
                destination
            )
        }

        SDL_RenderPresent(renderer)
    }

    override fun shutdown() {
        texture?.let {
            SDL_DestroyTexture(it)
            texture = null
        }

        if (renderer != 0L) {
            SDL_DestroyRenderer(renderer)
            renderer = 0L
        }

        if (window != 0L) {
            SDL_DestroyWindow(window)
            window = 0L
        }

        SDL_Quit()
    }

}

/*
* I am using SDL with lwjgl in kotlin for a chip 8 emulator. it currently is incorrect and seems to be drawing everything too far up and stretched. how fix?

package platform

import chip8.Display
import chip8.Keyboard
import org.lwjgl.sdl.SDL
import org.lwjgl.sdl.SDL_Event
import org.lwjgl.sdl.SDL_FRect
import org.lwjgl.sdl.SDLInit.*
import org.lwjgl.sdl.SDLRender.*
import org.lwjgl.sdl.SDLScancode.*
import org.lwjgl.sdl.SDLEvents.*
import org.lwjgl.sdl.SDLError.*
import org.lwjgl.sdl.SDLVideo.*
import org.lwjgl.system.MemoryStack

import kotlin.math.floor
import kotlin.math.min

class SDLPlatform(
    windowTitle: String,
    windowWidth: Int,
    windowHeight: Int
) : Platform {

    companion object {
        private const val MINIMUM_WINDOW_WIDTH = 320
        private const val MINIMUM_WINDOW_HEIGHT = 240

        private const val PIXEL_ON_R: Byte = 255.toByte()
        private const val PIXEL_ON_G: Byte = 255.toByte()
        private const val PIXEL_ON_B: Byte = 255.toByte()

        private const val PIXEL_OFF_R: Byte = 0
        private const val PIXEL_OFF_G: Byte = 0
        private const val PIXEL_OFF_B: Byte = 0

        private const val ALPHA: Byte = 255.toByte()

        private const val WINDOW_FLAGS = 0L
    }

    private var window: Long = 0L
    private var renderer: Long = 0L

    private val keyMap = mapOf(
        SDL_SCANCODE_1 to 0x1,
        SDL_SCANCODE_2 to 0x2,
        SDL_SCANCODE_3 to 0x3,
        SDL_SCANCODE_4 to 0xC,

        SDL_SCANCODE_Q to 0x4,
        SDL_SCANCODE_W to 0x5,
        SDL_SCANCODE_E to 0x6,
        SDL_SCANCODE_R to 0xD,

        SDL_SCANCODE_A to 0x7,
        SDL_SCANCODE_S to 0x8,
        SDL_SCANCODE_D to 0x9,
        SDL_SCANCODE_F to 0xE,

        SDL_SCANCODE_Z to 0xA,
        SDL_SCANCODE_X to 0x0,
        SDL_SCANCODE_C to 0xB,
        SDL_SCANCODE_V to 0xF
    )

    init {
        if (!SDL_Init(SDL_INIT_VIDEO)) {
            error("SDL_Init failed: ${SDL_GetError()}")
        }

        window = SDL_CreateWindow(
            windowTitle,
            windowWidth,
            windowHeight,
            WINDOW_FLAGS
        )

        if (window == 0L) {
            val errorMessage = SDL_GetError()

            SDL_Quit()

            error("Window creation failed: $errorMessage")
        }

        renderer = SDL_CreateRenderer(window, "")

        if (renderer == 0L) {
            val errorMessage = SDL_GetError()

            SDL_DestroyWindow(window)
            SDL_Quit()

            error("Renderer creation failed: $errorMessage")
        }

        SDL_SetWindowMinimumSize(
            window,
            MINIMUM_WINDOW_WIDTH,
            MINIMUM_WINDOW_HEIGHT
        )

        SDL_SetRenderVSync(renderer, 1)

        println("SDL initialized!")
    }

    override fun processInput(keyboard: Keyboard): Boolean {
        val event = SDL_Event.malloc()

        try {
            while (SDL_PollEvent(event)) {

                when (event.type()) {

                    SDL_EVENT_QUIT -> {
                        return false
                    }

                    SDL_EVENT_KEY_DOWN -> {
                        val key = keyMap[event.key().scancode()]

                        if (key != null) {
                            keyboard.setKey(key, true)
                        }
                    }

                    SDL_EVENT_KEY_UP -> {
                        val key = keyMap[event.key().scancode()]

                        if (key != null) {
                            keyboard.setKey(key, false)
                        }
                    }
                }
            }
        } finally {
            event.free()
        }

        return true
    }

    override fun render(display: Display) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255.toByte())
        SDL_RenderClear(renderer)
        
        MemoryStack.stackPush().use { stack ->
            val widthBuffer = stack.mallocInt(1)
            val heightBuffer = stack.mallocInt(1)

            SDL_GetWindowSizeInPixels(
                window,
                widthBuffer,
                heightBuffer
            )

            val windowWidth = widthBuffer[0]
            val windowHeight = heightBuffer[0]

            val scale = min(
                windowWidth / Display.WIDTH,
                windowHeight / Display.HEIGHT
            )

            // Avoid a zero-sized scale if the window becomes tiny.
            if (scale <= 0) {
                SDL_RenderPresent(renderer)
                return@use
            }

            val scaledWidth = Display.WIDTH * scale
            val scaledHeight = Display.HEIGHT * scale

            val offsetX = (windowWidth - scaledWidth) / 2
            val offsetY = (windowHeight - scaledHeight) / 2

            val rect = SDL_FRect.malloc(stack)

            rect.w(scale.toFloat())
            rect.h(scale.toFloat())

            for (y in 0 until Display.HEIGHT) {
                for (x in 0 until Display.WIDTH) {

                    if (!display.getPixel(x, y)) {
                        continue
                    }

                    SDL_SetRenderDrawColor(
                        renderer,
                        PIXEL_ON_R,
                        PIXEL_ON_G,
                        PIXEL_ON_B,
                        ALPHA
                    )

                    rect.x((offsetX + x * scale).toFloat())
                    rect.y((offsetY + y * scale).toFloat())

                    SDL_RenderFillRect(renderer, rect)
                }
            }
        }

        SDL_RenderPresent(renderer)
    }


    override fun shutdown() {
        if (renderer != 0L) {
            SDL_DestroyRenderer(renderer)
            renderer = 0L
        }

        if (window != 0L) {
            SDL_DestroyWindow(window)
            window = 0L
        }

        SDL_Quit()
    }


}
* */

