from time import sleep
from machine import Pin

led = Pin(25,Pin.OUT)

while True:
    led.value(1)
    print("Encendido")
    sleep(0.5)
    led.value(0)
    print("Apagado")
    print("Gay")
    sleep(0.5)
