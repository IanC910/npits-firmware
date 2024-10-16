# This module provides interfacing methods for the HC-SR04 ultrasonic sensor

from gpiozero import DigitalInputDevice, DigitalOutputDevice
import time

SPEED_OF_SOUND_CM_PER_S = 34300

class HCSR04:
    def __init__(self, trig_gpio, echo_gpio):
        self.trig_gpio = DigitalOutputDevice(trig_gpio)
        self.echo_gpio = DigitalInputDevice(echo_gpio)

    def get_distance_cm(self):
        # 10 µs pulse on trig pin
        self.trig_gpio.on()
        ten_us_in_s = 0.00001
        time.sleep(ten_us_in_s)
        self.trig_gpio.off()

        # Measure duration of pulse on echo pin
        pulse_start_s = None
        pulse_end_s = None

        while not self.echo_gpio.is_active:
            pulse_start_s = time.time()

        while self.echo_gpio.is_active:
            pulse_end_s = time.time()

        pulse_duration_s = pulse_end_s - pulse_start_s
        distance_cm = pulse_duration_s / 2 * SPEED_OF_SOUND_CM_PER_S

        return distance_cm
