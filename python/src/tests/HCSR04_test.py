
import time
import sys

sys.path.append('../')

import pin_defines
from devices.HCSR04 import HCSR04

def HCSR04_test():
    print("HC-SR04 Test")

    ultrasonic = HCSR04(pin_defines.HCSR04_TRIG_GPIO, pin_defines.HCSR04_ECHO_GPIO)

    while True:
        distance_cm = ultrasonic.get_distance_cm()

        print("Distance: %8.0f cm" % distance_cm)

        time.sleep(0.2)

if(__name__ == "__main__"):
    HCSR04_test()