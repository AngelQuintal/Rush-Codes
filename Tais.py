import network, urequests, utime, machine
from machine import RTC, I2C, Pin
from ssd1306 import SSD1306_I2C
from max30102 import MAX30102
from utime import ticks_diff, ticks_us

ssid = "Students" # No olvides colocar el nombre tu red WiFi
password = "P0l1t3cn1c4.b1s"  # No olvides colocar tu contraseña
url = "http://worldtimeapi.org/api/timezone/America/Mexico_City" # zonas en: http://worldtimeapi.org/timezones

print("Conectando ...")
scl = Pin(22)
sda = Pin(21)
i2c = I2C(scl=scl, sda=sda)
oled = SSD1306_I2C(128, 64, i2c)
oled.fill(0)
oled.text("Conectando ...", 0, 0)
oled.show()

rtc = RTC()

wifi = network.WLAN(network.STA_IF)
wifi.active(True)
wifi.connect(ssid, password)

while not wifi.isconnected():
    pass

print("IP:", wifi.ifconfig()[0], "\n")
oled.text("Conectado con IP: ", 0, 35)
oled.text(" " + str(wifi.ifconfig()[0]), 0, 45)
oled.show()

ultima_peticion = 0
intervalo_peticiones = 60  # en segundos

try:
    sensor2 = MAX30102(i2c=i2c)  
except RuntimeError as err:
    print("Error occurred while sensor initialization:", err)
    raise SystemExit(err)

sensor2.setup_sensor()
sensor2.set_sample_rate(800)
sensor2.set_fifo_average(8)

utime.sleep(1)

#print("Reading temperature in °C.", '\n')
#print(sensor2.read_temperature())
compute_frequency = True

utime.sleep(1)

t_start = ticks_us() 
samples_n = 0

while True:
    sensor2.check()
    if sensor2.available():
        red_reading = sensor2.pop_red_from_storage()
        ir_reading = sensor2.pop_ir_from_storage()
        
        bpm = ir_reading / 600 + 30
        spo2 = (red_reading * 98) / 32767
        
        ibpm = int(bpm)
        sbpm = str(ibpm)
        
        ispo2 = int(spo2)
        sspo2 = str(ispo2)
        
        itemp = int(sensor2.read_temperature())
        stemp = str(itemp)
    
    if not wifi.isconnected():
        print("fallo de conexión a WiFi")
        #machine.reset()
    
    if (utime.time() - ultima_peticion) >= intervalo_peticiones:
        response = urequests.get(url)
    
        if response.status_code == 200:
            print("Respuesta:\n", response.text)
            
            datos_objeto = response.json()
            fecha_hora = str(datos_objeto["datetime"])
            año = int(fecha_hora[0:4])
            mes = int(fecha_hora[5:7])
            día = int(fecha_hora[8:10])
            hora = int(fecha_hora[11:13])
            minutos = int(fecha_hora[14:16])
            segundos = int(fecha_hora[17:19])
            sub_segundos = int(round(int(fecha_hora[20:26]) / 10000))
        
            rtc.datetime((año, mes, día, 0, hora, minutos, segundos, sub_segundos))
            ultima_peticion = utime.time()
            print("RTC actualizado\n")
   
        else:
            print("respuesta no válida: RTC no actualizado")
    
    fecha_pantalla = "Fecha:{2:02d}/{1:02d}/{0:4d}".format(*rtc.datetime())
    hora_pantalla = "Hora: {4:02d}:{5:02d}:{6:02d}".format(*rtc.datetime())

    oled.fill(0)
    
    oled.text("BPM: ", 0,0)
    oled.text(sbpm, 50, 0)
    oled.text("SPo2: ", 0, 10)
    oled.text(sspo2, 50, 10)

    oled.text(fecha_pantalla, 0, 25)
    oled.text(hora_pantalla, 0, 45)
    oled.show()
    
    utime.sleep(0.1)
