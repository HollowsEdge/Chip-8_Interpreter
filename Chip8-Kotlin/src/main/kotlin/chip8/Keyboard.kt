package chip8


class Keyboard {
    private val keysPressedLastFrame: BooleanArray = BooleanArray(16)
    private val keysPressed: BooleanArray = BooleanArray(16)
    private val keysJustReleased: BooleanArray = BooleanArray(16)
    
    fun updateKeys(){
        keysJustReleased.fill(false)
        for ((i, keyPressedState) in keysPressed.withIndex()) {
            if (!keyPressedState && keysPressedLastFrame[i]){
                keysJustReleased[i] = true
            }
            keysPressedLastFrame[i] = keyPressedState
        }
    }
    
    fun setKey(keyIndex: Int, newValue: Boolean) {
        keysPressed[keyIndex] = newValue
    }
    
    fun getKeyDown(index: Int): Boolean {
        return keysPressed[index]
    }

    fun getKeyJustReleased(index: Int): Boolean {
        return keysJustReleased[index]
    }
    
    fun getAnyKeyReleased(): Int {
        return keysJustReleased.indexOf(true)
    }
}