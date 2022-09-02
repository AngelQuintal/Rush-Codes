from machine import Pin
from time import sleep
led = Pin(25, Pin.OUT)
n = 0

while True:
    led.toggle()
    print("13 x {} = {}".format(n, 13*n)) # print the thirteen-times table
    n = n + 1
    sleep(0.5)
