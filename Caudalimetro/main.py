###############Librerias###############################
from machine import Pin, I2C, RTC, Timer, SPI, SoftSPI
import utime
from time import sleep_ms
import ds3231 #3V3
import os
from sdcard import SDCard
import _thread

# write_time_c = 20
write_time_c = 5
i = 0
andycounter = 0

################# Used Pins ##############################
scl = Pin(1)
sda = Pin(0)
i2c = I2C(0,scl=Pin(1), sda=Pin(0)) #Agregar direccion I2C correspondiendo al puerto en los pines (0)
ds = ds3231.DS3231(i2c)
sf = Pin(29, Pin.IN)

spisd = SoftSPI(-1, miso=Pin(8), mosi=Pin(7), sck=Pin(6))
sd = SDCard(spisd, Pin(5))

##################### Interrup Caud ##########################
Q = 0
np = 0 # Numero de pulsos
reloj = Timer() #Ayuda a determinar la frecuencia (pulsos por segundo

##################### Escritura microSD ##########################
def sdcard_write (qval):
        day = (ds.get_time()[2])
        month = (ds.get_time()[1])
        year = (ds.get_time()[0])
        hour = (ds.get_time()[3])
        minute = (ds.get_time()[4])
        second = (ds.get_time()[5])

        sday = str(day)
        smonth = str(month)
        syear = str(year)
        shour = str(hour)
        sminute = str(minute)
        ssecond = str(second)

        fQ = float(qval)
        sQ = str(fQ)

        f = open('CAUD1.txt', 'a')
        f.write(sday + '/' + smonth + '/' + syear + '      ' + shour + ':' + sminute + ':' + ssecond + '          ' + 'Q='+ sQ + '\n')
        f.close()
        print("Data was sent")
        utime.sleep(0.5)
       # andycounter+=1
        # if andycounter>20
            # break
    # andycounter=0
    
def conteo(pin):
    global np
    np += 1 #Cada que detecte un pulso es +1


def freq(timer):
    global np, Q, i, write_time_c #np es no. de pulsos y Q es caudal
    frec = np #Se desborda cada 1s
    Q = frec / 7.5 #Flujo como Q (litros por min)
    i += 1
    if i == write_time_c:
        sdcard_write(Q)
        i = 0
    print (f"f= {frec} y Q= {Q}")
    print (i)
    print(np)
    
    # np = 0  #Cada segundo se regresa a pulso 0 para contar de nuevo


##################### SDCard data##########################
print('Root directory:{}'.format(os.listdir()))
vfs = os.VfsFat(sd)
os.mount(vfs, '/sd')
print('Root directory:{}'.format(os.listdir()))
os.chdir('sd')
print('SD Card contains:{}'.format(os.listdir()))

#open('CAUD1.txt', 'w').close()

sf.irq(trigger = Pin.IRQ_RISING, handler = conteo) #Deteccion por cada HIGH
reloj.init(mode= Timer.PERIODIC, period= 1000, callback= freq) #Timer de 1s

while True:
    pass