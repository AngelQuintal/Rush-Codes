import ds1307
from machine import Pin, I2C

id = 0
sda = Pin(0)
scl = Pin(1)

i2c = I2C(id=id, scl=scl, sda=sda)
ds = ds1307.DS1307(i2c)

while True:
    print(ds.datetime())