from machine import Pin
from time import sleep

SW = Pin(18, Pin.IN, Pin.PULL_UP)
DT = Pin(17, Pin.IN)
CLK = Pin(16, Pin.IN)

previous_value = True
button_down = False

while True:
    if previous_value != CLK.value():
        if CLK.value() == False:

            if DT.value() == False:
                print("Izquierda")
                sleep(0.2)
            else:
                print("Derecha")
                sleep(0.2)
            previous_value = CLK.value() 
        
    if SW.value() == True and not button_down:
        button_down = False