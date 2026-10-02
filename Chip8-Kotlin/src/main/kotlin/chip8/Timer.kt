package chip8

class Timer {
    private var timerValue: Int = 0
    fun setTimer(newTime: Int){
        timerValue = newTime
    }
    
    fun getTime(): Int {
        return timerValue
    }
    
    fun tickTimer(){
        if(timerValue > 0)
            timerValue -= 1
    }
}