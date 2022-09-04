import utime
import machine
from machine import I2C
from lcd_api import LcdApi
from pico_i2c_lcd import I2cLcd

#The address is according to the model, in a 16x2 LCD is 0x20
#This is for a 20x4 lcd
I2C_ADDR     = 0x27
I2C_NUM_ROWS = 4
I2C_NUM_COLS = 20

#LCD configuration on the ESP32 pins
i2c = I2C(sda=machine.Pin(21), scl=machine.Pin(22), freq=400000)
lcd = I2cLcd(i2c, I2C_ADDR, I2C_NUM_ROWS, I2C_NUM_COLS)

#(x,y) on the LCD, x=(0-19)row, y=(0-3)column
while True:
        lcd.clear()
        lcd.move_to(0, 0)
        lcd.putstr("a")

        lcd.move_to(0, 1)
        lcd.putstr("b")

        lcd.move_to(0, 2)
        lcd.putstr("c")

        lcd.move_to(0, 3)
        lcd.putstr("d")

        utime.sleep(2)


# The following line of code should be tested
# using the REPL:

# 1. To print a string to the LCD:
#	 lcd.putstr('Hello world')
# 2. To clear the display:
#	 lcd.clear()
# 3. To control the cursor position:
#	 lcd.move_to(2, 1)
# 4. To show the cursor:
#	 lcd.show_cursor()
# 5. To hide the cursor:
#	 lcd.hide_cursor()
# 6. To set the cursor to blink:
#	 lcd.blink_cursor_on()
# 7. To stop the cursor on blinking:
#	 lcd.blink_cursor_off()
# 8. To hide the currently displayed character:
#	 lcd.display_off()
# 9. To show the currently hidden character:
#	 lcd.display_on()
# 10. To turn off the backlight:
#	 lcd.backlight_off()
# 11. To turn ON the backlight:
#	 lcd.backlight_on()
# 12. To print a single character:
#	 lcd.putchar('x')
# 13. To print a custom character:
#	 happy_face = bytearray([0x00, 0x0A, 0x00, 0x04, 0x00, 0x11, 0x0E, 0x00])
#	 lcd.custom_char(0, happy_face)
#	 lcd.putchar(chr(0))
