from machine import Pin, Timer
led = Pin(2, Pin.OUT)
#led2 = Pin(22, Pin.OUT)
timer = Timer()
#bro = Timer()
def blink(timer):
    led.toggle()

timer.init(freq=12.5, mode=Timer.PERIODIC, callback=blink)

def blink(bro):
        led2.toggle()
timer.init(freq=2.5, mode=Timer.PERIODIC, callback=blink)