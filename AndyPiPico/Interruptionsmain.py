from machine import Pin
from time import sleep_ms
# from ssd1306 import SSD1306_I2C
# import framebuf

ANCHO=128
ALTO=64

MODO_MAX=5

global modo
global contador

modo=0
contador=0

boton=Pin(8,Pin.IN,Pin.PULL_DOWN)
#i2c=I2C(1,scl=Pin(14),sda=Pin(15))
# oled=SSD1306_I2C(ANCHO,ALTO,i2c)

def boton_interrupt(pin):
    global modo
    global contador
    
    if boton.value():
        modo+=1
        print(modo)
        
    if modo>MODO_MAX:
        modo=0
        contador=0
        
boton.irq(trigger=Pin.IRQ_RISING,handler=boton_interrupt)

while True:
#     oled.fill(0)
     sleep_ms(10)
     if modo==0:
         print("pantalla 0") # print the thirteen-times table
         
# #         oled.text("Bienvenidos",0,0)
# #         oled.text("Purbea de texto",0,16)
# #         oled.text("Like", 30,32)
#         
     elif modo==1:
        print("pantalla 1") # print the thirteen-times table
# #         oled.text("Variables",0,0)
# #         oled.text("contando:", 10,32)
# #         oled.text(str(contador),90,32)
# #         oled.show()
#         #sleep_ms(1000)
#         #contador+=1
#     
     elif modo==2:
        print("Pantalla 2") # print the thirteen-times table
# #         for i in range(0,63,8):
# #             oled.vline(i,0,63,1)
#             
     elif modo==3:
        print("Pantalla 3") # print the thirteen-times table
# #         for i in range(0,127,8):
# #             oled.vline(i,0,63,1)
#        
     elif modo==4:
        print("Pantalla 4") # print the thirteen-times table
# #         oled.rect(10,10,40,20,1)
# #         oled.rect(60,30,50,20,1)
#         
     elif modo==5:
        print("Pantalla 5") # print the thirteen-times table
# #         oled.fill_rect(10,10,40,20,1)
# #         oled.fill_rect(60,30,50,20,1)
# #         oled.fill_rect(62,32,20,10,0)
# #         oled.fill_rect(87,38,20,10,0)
#         
# #     oled.show()

