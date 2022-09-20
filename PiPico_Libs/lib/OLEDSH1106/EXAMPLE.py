# Mostrar imagen y texto en la pantalla OLED sh1106 por comunicación I2C
from machine import Pin, I2C
from sh1106 import SH1106_I2C
import framebuf

WIDTH  = 128                                            # ancho de pantalla OLED
HEIGHT = 64                                             # altura de la pantalla OLED

i2c = I2C(1, scl=Pin(15), sda=Pin(14), freq=200000)       # Iniciar I2C usando pines GP14 y GP15 (pines I2C1 predeterminados)
print("I2C Address      : "+hex(i2c.scan()[0]).upper()) # Mostrar la dirección del dispositivo
print("I2C Configuration: "+str(i2c))                   # Mostrar configuración I2C


oled = SH1106_I2C(WIDTH, HEIGHT, i2c)                  # Pantalla inicializada

#Logotipo de Raspberry Pi como bytearray 32x32
buffer = bytearray(b"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00|?\x00\x01\x86@\x80\x01\x01\x80\x80\x01\x11\x88\x80\x01\x05\xa0\x80\x00\x83\xc1\x00\x00C\xe3\x00\x00~\xfc\x00\x00L'\x00\x00\x9c\x11\x00\x00\xbf\xfd\x00\x00\xe1\x87\x00\x01\xc1\x83\x80\x02A\x82@\x02A\x82@\x02\xc1\xc2@\x02\xf6>\xc0\x01\xfc=\x80\x01\x18\x18\x80\x01\x88\x10\x80\x00\x8c!\x00\x00\x87\xf1\x00\x00\x7f\xf6\x00\x008\x1c\x00\x00\x0c \x00\x00\x03\xc0\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00")
# Cargue el logo de Raspberry Pi en el framebuffer (la imagen es 32x32)
fb = framebuf.FrameBuffer(buffer,32, 32, framebuf.MONO_HLSB)
oled.rotate(0x01) #pone la screen de manera normal
# Borre la pantalla OLED en caso de que tenga basura.
oled.fill(0)

# Posicion de la imagen del framebuffer a la pantalla oled
oled.blit(fb, 94, 0)

#Añadir texto(ancho,alto)
oled.text("Raspberry Pi",5,5)
oled.text("Pico",5,15)
oled.text("Microelectronica",0,40)
oled.text("Lab",105,50)
# Actualice finalmente la pantalla oled para que se muestren la imagen y el texto
oled.show()
