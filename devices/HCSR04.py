
# This module provides interfacing functions for the HCSR04 ultrasonic sensor

import RPi.GPIO as gpio
import time

SPEEF_OF_SOUND_CM_PER_S = 34300

class HCSR04:
    def __init__(trig_gpio, echo_gpio):
        self.trig_gpio = trig_gpio
        self.echo_gpio = echo_gpio

        gpio.setmode(gpio.BCM)
        gpio.setup(trig_gpio, gpio.OUT)
        gpio.setup(echo_gpio, gpio.IN)

    def get_distance_cm():
        # 10 us pulse on trig pin
        gpio.output(trig_gpio, gpio.HIGH)
        ten_us_in_s = 0.00001
        time.sleep(ten_us_in_s)
        gpio.output(trig_gpio, gpio.LOW)

        # Measure duration of pulse on echo pin
        while(gpio.input(echo_gpio) == 0):
            pulse_start_s = time.time()

        while(gpio.input(echo_gpio) == 1):
            pulse_end_s = time.time()

        pulse_duration_s = pulse_end_s - pulse_start_s
        distance_cm = pulse_duration_s / 2 * SPEEF_OF_SOUND_CM_PER_S

        return distance_cm