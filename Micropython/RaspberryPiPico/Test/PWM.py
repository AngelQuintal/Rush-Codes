from machine import Pin, PWM
import time

pwm = PWM(Pin(2))
pwm.duty(896)
time.sleep(1)
pwm.duty(512)
time.sleep(1)
pwm.duty(0)