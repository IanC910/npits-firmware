
# This test tries to get device info from the K-LD2 doppler radar via
# via the rpi's ttyS0 serial device

# trig: pin 18 = gpio 24
# echo: pin 16 = gpio 23

import RPi.GPIO as gpio
import time
from time import sleep

def ultrasonic_test():
    print("Ultrasonic Test")

    SPEED_OF_SOUND_CM_PER_S = 34300

    trig_gpio = 24
    echo_gpio = 23

    gpio.setmode(gpio.BCM)
    gpio.setup(trig_gpio, gpio.OUT)
    gpio.setup(echo_gpio, gpio.IN)

    while True:
        gpio.output(trig_gpio, gpio.LOW)
        sleep(2 / 1000000)

        gpio.output(trig_gpio, gpio.HIGH)
        sleep(10 / 1000000)
        gpio.output(trig_gpio, gpio.LOW)

        while(gpio.input(echo_gpio) == 0):
            pulse_start = time.time()
        while(gpio.input(echo_gpio) == 1):
            pulse_end = time.time()

        pulse_duration = pulse_end - pulse_start
        distance_cm = pulse_duration * SPEED_OF_SOUND_CM_PER_S / 2

        print(distance_cm)

        sleep(0.1)

if(__name__ == "__main__"):
    ultrasonic_test()