# More details can be found in TechToTinker.blogspot.com 
# George Bantique | tech.to.tinker@gmail.com

from machine import Pin
from machine import SoftI2C 
from ds3231 import DS3231

i2c = SoftI2C(scl=Pin(1), sda=Pin(0), freq=400000) 
ds = DS3231(i2c)


# The following line of codes can be tested using the REPL:
# 1. Get the current time
ds.get_time()
# Return:
#        YYYY, MM, DD, HH, mm, ss, WD, YD
#        YYYY - year
#          MM - month
#          DD - day
#          HH - hour in 24 hour
#          mm - minutes
#          ss - seconds
#          WD - week day: 1=Monday, 7=Sunday
#          YD - day of the year
# 2. Set the current time
# ds.set_time(YYYY, MM, DD, HH, mm, ss, WD, YD)
ds.set_time(2022, 10, 28, 17, 29, 00, 5, 00)
ds.get_time()