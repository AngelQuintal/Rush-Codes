import os
from machine import Pin, SoftSPI
import utime 
import dht
from sdcard import SDCard
import _thread

sensor = dht.DHT11(Pin(28))

# Pin assignment:
# MISO -> GPIO 12
# MOSI -> GPIO 11
# SCK  -> GPIO 10
# CS   -> GPIO 13
# VCC  -> 5V
# GND -> GND

spisd = SoftSPI(-1, miso=Pin(12), mosi=Pin(11), sck=Pin(10))
sd = SDCard(spisd, Pin(13))

print('Root directory:{}'.format(os.listdir()))
vfs = os.VfsFat(sd)
os.mount(vfs, '/sd')
print('Root directory:{}'.format(os.listdir()))
os.chdir('sd')
print('SD Card contains:{}'.format(os.listdir()))

def sdcard_write ():
    while True:
        
        temp = sensor.temperature()
            
        ftemp = float(temp)
        stemp = str(ftemp)
        
        f = open('Temp_test1.txt', 'a')
        f.write(stemp + '\n')
        f.close()
        
        print("Data was sent")
        
        utime.sleep(5)
_thread.start_new_thread(sdcard_write,())

while True:
  try:
    sensor.measure()
    temp = sensor.temperature()

    print('Temperature: %3.1f C' %temp)
    print('')
    utime.sleep(5) #5 segundo
    
  except OSError as e:
    print('Failed to read sensor.')